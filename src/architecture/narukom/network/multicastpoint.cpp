
#include "multicastpoint.hpp"

#include "msgentryserialize.hpp"
#include <boost/functional/hash.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <google/protobuf/descriptor.h>
#include "messages/Network.pb.h"
#include "architecture/narukom/pub_sub/buffer.h"
#include "architecture/narukom/pub_sub/topicTree.h"

namespace KNetwork
{


MulticastPoint::MulticastPoint(std::string const& name,unsigned payloadsize) :
	EndPoint(name), Thread(false) ,
	io(),multisocket(io),timer_(io),payloadsize_(payloadsize),cleanupandbeacon(10)
{
	//hash time and produce string
	boost::hash<std::string> h;
	boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
	thishost=h(boost::posix_time::to_iso_string(now));
	p.setHost(thishost );

	std::cout<<"Multicast hostid:"<<thishost<<std::endl;

};

MulticastPoint::~MulticastPoint()
{
	this->StopThread();

}

int MulticastPoint::startEndPoint(std::string const& multicast_ip,unsigned int port)
{
	boost::asio::ip::address multicast_address(boost::asio::ip::address::from_string(multicast_ip));

	multicast_point_= boost::asio::ip::udp::endpoint(multicast_address,port);

	boost::asio::ip::udp::endpoint rpoint(boost::asio::ip::address_v4::any(), port);

    multisocket.open(rpoint.protocol());
	multisocket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	//remotesocket.set_option(boost::asio::ip::multicast::enable_loopback(false));

    multisocket.bind(rpoint);

    multiport=port;
    localport=multisocket.local_endpoint().port();

    // Join the multicast group.
    multisocket.set_option( boost::asio::ip::multicast::join_group(multicast_address));

    queue_receive();
    timer_.expires_from_now(boost::posix_time::milliseconds(100));
	timer_.async_wait(
		  boost::bind(&MulticastPoint::handle_timeout, this,
			boost::asio::placeholders::error));

	this->StartThread();
	if(getReadBuffer())
		getReadBuffer()->setNotifier(boost::bind(&MulticastPoint::bufferCallback,this,_1));

}

void MulticastPoint::setCleanupAndBeacon(unsigned i)
{
	cleanupandbeacon=i;
}

void MulticastPoint::queue_receive()
{

	void *b=dep.getbuffer();
	multisocket.async_receive_from(
	boost::asio::buffer(b, dep.getbufferSize()), sender_endpoint_,
	boost::bind(&MulticastPoint::handle_receive_from, this,(const char *)b,
	boost::asio::placeholders::error,
	boost::asio::placeholders::bytes_transferred));
}



void MulticastPoint::handle_receive_from(const char* buffer,const boost::system::error_code& error,size_t bytes_recvd)
{
	//std::cout<<"Receive"<<std::endl;
	if (!error)
    {
    	queue_receive();

    	processIncoming(buffer,bytes_recvd);
    	dep.releaseBuffer(buffer);

    }

}


void MulticastPoint::handle_send_to(const boost::system::error_code& error)
{
	if (error)
    {
		std::cout<<error<<std::endl;

    }

}


void MulticastPoint::handle_timeout(const boost::system::error_code& error)
{
	if (!error)
	{

		{

			HostSubscriptions *hs=HostSubscriptions::default_instance().New();

			std::set< std::pair<hostid,std::size_t> >::iterator sit=remoteSubscriptions.begin();
			for(;sit!=remoteSubscriptions.end();++sit)
			{
				Subscription *s=hs->add_topics();
				s->set_host((*sit).first);
				s->set_topicid((*sit).second);
			}
			msgentry m;
			m.msg.reset(hs);
			m.topic=Topics::Instance().getId("communication");
			m.msgclass=msgentry::STATE;
			m.timestamp=boost::posix_time::microsec_clock::universal_time();
			m.host=msgentry::HOST_ID_LOCAL_HOST;

			processOutGoing(m);
		}

		{

			KnownHosts *kn=KnownHosts::default_instance().New();
			std::set<hostDescription>::iterator kit=otherHosts.begin();
			for(;kit!=otherHosts.end();++kit)
			{
				kn->add_name((*kit).h);

			}
			msgentry m;
			m.msg.reset(kn);
			m.topic=Topics::Instance().getId("communication");
			m.msgclass=msgentry::STATE;
			m.timestamp=boost::posix_time::microsec_clock::universal_time();
			m.host=msgentry::HOST_ID_LOCAL_HOST;
			publish(m);
		}



		timer_.expires_from_now(boost::posix_time::milliseconds(cleanupandbeacon));
		timer_.async_wait(
		  boost::bind(&MulticastPoint::handle_timeout, this,
			boost::asio::placeholders::error));



	}
}

int MulticastPoint::Execute()
{

	io.run_one();

}

packet msgentryToBytes(msgentry const& m)
{

	size_t msgbytes=m.msg->ByteSize();
	size_t totalsize=msgbytes;
	totalsize+=m.msg->GetTypeName().size();
	totalsize+=sizeof(serializedmsgheader);
	char *r=new char[totalsize];
	char *d=r;
	serializedmsgheader *h=(serializedmsgheader *)d;
	h->encodeFromMsg(m,m.msg->GetTypeName().size());
	d+=sizeof(serializedmsgheader);
	memcpy(d,m.msg->GetTypeName().data(),m.msg->GetTypeName().size());
	d+=m.msg->GetTypeName().size();
	m.msg->SerializePartialToArray(d,msgbytes);
	packet p;
	p.bytes=r;
	p.size=totalsize;
	return p;
}
using namespace google::protobuf;
bool msgentryFromBytes(packet const& p,msgentry &m)
{
	const serializedmsgheader *h=(const serializedmsgheader*)p.bytes;
	m=h->decodeMsg();
	std::string TypeName=std::string(p.bytes+sizeof(serializedmsgheader),h->getTypeData());
	const Descriptor *d=DescriptorPool::generated_pool()->FindMessageTypeByName(TypeName);
	if(!d)
	{
		std::cout<<"Could Not Find Descriptor for:"<<TypeName<<std::endl;
		return false;
	}
	Message *protomsg=MessageFactory::generated_factory()->GetPrototype(d)->New();
	if( !protomsg->ParsePartialFromArray(
				p.bytes+sizeof(serializedmsgheader)+h->getTypeData(),
				p.size-(sizeof(serializedmsgheader)+h->getTypeData())
						) )
	{
		std::cout<<"Cannot Parse Message of type :"<<TypeName<<std::endl;
	}

	m.msg.reset(protomsg);
	//std::cout<<m.msg.get()<<std::endl;
	m.host=msgentry::HOST_ID_LOCAL_HOST;
	return true;
}

void MulticastPoint::bufferCallback(MessageBuffer *mbuf)
{
	std::vector<msgentry> v=remove();
	for(unsigned i=0;i<v.size();i++)
	{
		io.post(boost::bind(&MulticastPoint::processOutGoing,this,v[i]) );
	}

}
void MulticastPoint::processOutGoing(msgentry m)
{
	if(m.msgclass>=msgentry::SUBSCRIBE_ON_TOPIC &&m.msgclass<=msgentry::UNSUBSCRIBE_ALL_TOPIC)
	{

		bool actionadd=m.msgclass>=msgentry::SUBSCRIBE_ON_TOPIC&&m.msgclass<=msgentry::SUBSCRIBE_ALL_TOPIC;
		if(m.host==msgentry::HOST_ID_LOCAL_HOST||m.host==thishost)
			return;
		std::set<std::size_t> topics=Topics::Instance().iterateTopics(m.topic,m.msgclass);
		std::set<std::size_t>::iterator it= topics.begin();
		for(;it!=topics.end();++it)
		{
			if(actionadd)
				remoteSubscriptions.insert(std::pair<hostid,std::size_t>(m.host,*it));
			else
				remoteSubscriptions.erase(std::pair<hostid,std::size_t>(m.host,*it));

			std::cout<<"Subscription:"<<m.host<<","<<(*it)<<" doing:"<<actionadd<<std::endl;
		}


		return;
	}

	if(m.msg==NULL)
		return;

	packet pack=msgentryToBytes(m);//Serialize msgentry
	RawPacketBatch b=p.feed(pack.bytes,pack.size);//Packetize data

	delete[] pack.bytes;
	//std::cout<<"New set"<<std::endl;
	for( unsigned i=0;i<b.data.size();i++)
	{
		//std::cout<<"Sending:"<<b.data[i].size<<m.msg->GetTypeName()<<std::endl;

		multisocket.async_send_to(
		boost::asio::buffer(b.data[i].bytes,b.data[i].size), multicast_point_,
					boost::bind(&MulticastPoint::handle_send_to, this,
					boost::asio::placeholders::error));
	}

}

void MulticastPoint::processIncoming( const char * buff, size_t size)
{
	RawDepacketizer::depacketizer_result  r=dep.feed(buff,size);
	if(r.host<=1)
	{
		if(r.p.bytes!=NULL)
			delete[] r.p.bytes;
		return;
	}

	//std::cout<<"host:"<<r.host<<std::endl;
	msgentry m;
	if(!msgentryFromBytes(r.p,m))
	{
		if(r.p.bytes!=NULL)
			delete[] r.p.bytes;
		return;
	}

	delete[] r.p.bytes;

	if(m.msg->GetTypeName()=="HostSubscriptions"&& r.host!=thishost)
	{
		/*
		KnownHosts *kn=KnownHosts::default_instance().New();
		std::set<hostid>::iterator kit=knownHosts.begin();
		for(;kit!=knownHosts.end();++kit)
		{
			kn->add_name(*kit);

		}
		msgentry m;
		m.msg.reset(kn);
		m.topic=Topics::Instance().getId("communication");
		m.msgclass=msgentry::STATE;
		m.timestamp=boost::posix_time::microsec_clock::universal_time();
		m.host=msgentry::HOST_ID_LOCAL_HOST;
		publish(m);
		*/
		hostDescription hd;
		hd.h=r.host;
		hd.lastseen=boost::posix_time::microsec_clock::universal_time();
		hd.timecorrection=hd.lastseen-m.timestamp;

		otherHosts.erase(hd);
		otherHosts.insert(hd);
		//std::cout<<"Found host:"<<r.host<<std::endl;
		std::vector<msgentry> newsubscriptions;
		msgentry kh;
		kh.msgclass=msgentry::SUBSCRIBE_ON_TOPIC;
		kh.host=msgentry::HOST_ID_LOCAL_HOST;

		boost::shared_ptr<const HostSubscriptions> hs=boost::static_pointer_cast<const HostSubscriptions>(m.msg);
		const ::google::protobuf::RepeatedPtrField< ::Subscription >& fptr=hs->topics();
		::google::protobuf::RepeatedPtrField< ::Subscription >::const_iterator cit;
		for(cit=fptr.begin();cit!=fptr.end();++cit)
		{
			if((*cit).host()==thishost||(*cit).host()==msgentry::HOST_ID_ANY_HOST)
			{
				if(localsubscriptions.find((*cit).topicid())!=localsubscriptions.end()) //Already subscribed
					continue;
				std::cout<<"Host:"<<r.host<<" needs topic:"<<(*cit).topicid()<<std::endl;
				kh.topic=(*cit).topicid();
				localsubscriptions.insert((*cit).topicid());
				newsubscriptions.push_back(kh);
			}

		}
		publish(newsubscriptions);

	}
	if(m.msg->GetTypeName()=="KnownHosts")//Who is that guy trying to pollute my host?
		return ;
	hostDescription hd;
	hd.h=r.host;
	if(otherHosts.find(hd)==otherHosts.end())//Unknown Host, reject
	{
		return;
	}
	//std::cout<<"Host:"<<r.host<<","<<m.topic<<std::endl;
	hostid anyhost=msgentry::HOST_ID_ANY_HOST;
	if(remoteSubscriptions.find(std::pair<hostid,std::size_t>(r.host,m.topic))==remoteSubscriptions.end() //Subscription to this host
		&&
		remoteSubscriptions.find(std::pair<hostid,std::size_t>(anyhost,m.topic))==remoteSubscriptions.end() //Or to any host
		)
		return;

	m.timestamp+=(*otherHosts.find(hd)).timecorrection;//Correct timestamp
	m.host=r.host;//Fix Host
	//std::cout<<"Received:"<<m.msg->GetTypeName()<<std::endl;
	publish(m);

}

}
