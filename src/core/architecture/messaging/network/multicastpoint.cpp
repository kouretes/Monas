
#include "multicastpoint.hpp"
#include "core/include/Logger.hpp"
#include "core/architecture/time/TimeTypes.hpp"
#include "core/architecture/messaging/serialization/MessageEntrySerialization.hpp"
#include <boost/functional/hash.hpp>
#include <google/protobuf/descriptor.h>
#include "core/messages/Network.pb.h"
#include "core/architecture/messaging/MessageBuffer.hpp"
#include "core/architecture/messaging/TopicTree.hpp"

class MessageEntrySerializationTraits
{
    public:
    typedef uint8_t topic_t;
    typedef KSystem::Time::TimeStamp timestamp_t;
    typedef uint8_t typename_t;
    static const bool hasTimeStamp=true;
    static const bool hasTopic=true;

};
namespace KNetwork
{
	using namespace KSystem;
	using namespace Messaging;

	static const hostid anyhost = Messaging::MessageEntry::HOST_ID_ANY_HOST;


	MulticastPoint::MulticastPoint(std::string const& name, unsigned payloadsize) :
		EndPoint(name), SystemThread(false) ,
		rio(), sio(), senderwork(sio), multireceive(rio),
		multisend(sio), timer_(rio), payloadsize_(payloadsize),
		cleanupandbeacon(10), otherHosts(), localsubscriptions(), uni(boost::mt19937(),  boost::uniform_real<>(0, 1))
	{
		//hash time and produce string
		boost::hash<KSystem::Time::TimeAbsolute::rep> h;
		KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();
		thishost = h((now).raw()); //Generate random hostid from current time
		dep.setHost(thishost); //My host id is used to reject loopback messages (if received==me, reject)
		p.setHost(thishost );  //My host id is used to tag multicast messages
		LogEntry(LogLevel::Info,"Multicast")<< "Multicast hostid:" <<(thishost);
		//std::cout<<"Multicast hostid:"<<thishost<<std::endl;
		//Initialize anyhost record;
		otherHosts[anyhost].lastseen = now;
	};

	MulticastPoint::~MulticastPoint()
	{
		LogEntry(LogLevel::Info,"Multicast")<<"Stopping Send Service";
		sio.stop();
		sendthread.join();
		LogEntry(LogLevel::Info,"Multicast")<<"Stopping Receive Service";
		rio.stop();
		this->StopThread();
		this->JoinThread();
		LogEntry(LogLevel::Info,"Multicast")<<"All went well";
	}

	void MulticastPoint::cleanupLocalSubscriptions()
	{

		std::map<hostid, hostDescription>::iterator  hit = otherHosts.begin();

		std::set<size_t>  remSub;
		std::set<size_t>::iterator sit;
		//Iterate all needed topics
		remSub = localsubscriptions;
		//std::cout<<"Unsubscribe:"<<std::endl;
		for( hit = otherHosts.begin(); hit != otherHosts.end(); hit++)
			for(sit = (*hit).second.needsTopics.begin(); sit != (*hit).second.needsTopics.end(); ++sit)
				remSub.erase(*sit);

		for(sit = remSub.begin(); sit != remSub.end(); ++sit)
			localsubscriptions.erase(*sit);

		std::vector<Messaging::MessageEntry> newsubscriptions;
		Messaging::MessageEntry kh;
		kh.msgclass = Messaging::MessageEntry::UNSUBSCRIBE_ON_TOPIC;
		kh.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;

		for(sit = remSub.begin(); sit != remSub.end(); ++sit)
		{
			kh.topic = (*sit);
			newsubscriptions.push_back(kh);
			//std::cout<<"Unsubscribe from:"<<(*sit)<<std::endl;
		}

		publish(newsubscriptions);


	}

	bool MulticastPoint::startEndPoint(std::string const& multicast_ip, unsigned int port)
	{
		try
		{
			boost::asio::ip::address multicast_address(boost::asio::ip::address::from_string(multicast_ip));
			multicast_point_ = boost::asio::ip::udp::endpoint(multicast_address, port);
			boost::asio::ip::udp::endpoint rpoint(boost::asio::ip::address_v4::any(), port);
			multireceive.open(rpoint.protocol());
			multisend.open(rpoint.protocol());
			multireceive.set_option(boost::asio::ip::udp::socket::reuse_address(true));
			multireceive.set_option(boost::asio::ip::multicast::enable_loopback(true));
			//remotesocket.set_option(boost::asio::ip::multicast::enable_loopback(false));
			multireceive.bind(rpoint);
			multiport = port;
			// Join the multicast group.
			multireceive.set_option( boost::asio::ip::multicast::join_group(multicast_address));
			multireceive.set_option( boost::asio::ip::multicast::hops(2));//Limit to two hop only, ie local+1
			queue_receive();
			timer_.expires_from_now(boost::posix_time::milliseconds(100));
			timer_.async_wait(
			    boost::bind(&MulticastPoint::handle_timeout, this,
			                boost::asio::placeholders::error));
			avgqueuesize = 0;
			queuesize = 0;
			canWarn = true; //Enable send error warnings
			sendthread = boost::thread(boost::bind(&boost::asio::io_service::run, &sio)); //Start Send
			this->StartThread(); //Start receive

			//Attach callbackfunction to incoming messages buffer
			if(getReadBuffer())
				getReadBuffer()->setNotifier(boost::bind(&MulticastPoint::bufferCallback, this, _1));
		}
		catch (boost::system::system_error e)
		{
			LogEntry(LogLevel::Error,"Multicast")<< "Could not start multicast networking!";
			return false;
		}

		return true;
	}

	void MulticastPoint::setCleanupAndBeacon(unsigned i)
	{
		cleanupandbeacon = i;
	}

	void MulticastPoint::queue_receive()
	{
		void *b = dep.getbuffer();
		multireceive.async_receive_from(
		    boost::asio::buffer(b, dep.getbufferSize()), sender_endpoint_,
		    boost::bind(&MulticastPoint::handle_receive_from, this, (const uint8_t *)b,
		                boost::asio::placeholders::error,
		                boost::asio::placeholders::bytes_transferred));
	}



	void MulticastPoint::handle_receive_from(const uint8_t* buffer, const boost::system::error_code& error, size_t bytes_recvd)
	{
		//std::cout<<"Receive"<<std::endl;
		if (!error)
		{
			queue_receive();
			//Gives possetion of aquired  buffer to Depacketizer
			processIncoming(buffer, bytes_recvd);
		}
	}


	void MulticastPoint::handle_send_to(const uint8_t* bytes, std::size_t size)
	{
		// Random early detect
		avgqueuesize = queuesize * wq + avgqueuesize * (1 - wq);
		p.assign(bytes, size);
		int s = p.nextPacket();

		while(s > 0)
		{
			try
			{
				 multisend.send_to(boost::asio::buffer(p.buff, s), multicast_point_);
			}
			catch (boost::system::system_error &r)
			{
				if(canWarn)//It is reset every beacon interval
				{
					LogEntry(LogLevel::Warning,"Multicast")<< "Possible Network Error";
					canWarn = false; //Disable warnings again
				}
			}

			s = p.nextPacket();
		}

		{
			SystemMutex::scoped_lock data_lock(mut);
			queuesize--;
		}

		delete[] bytes;
	}





	void MulticastPoint::handle_timeout(const boost::system::error_code& error)
	{
		if (!error)
		{
			//std::cout<<"timeout"<<std::endl;
			canWarn = true; //Re enable send error warnings
			KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();
			{
				//Clean old hosts first
				std::map<hostid, hostDescription>::iterator  t, hit = otherHosts.begin();
				KSystem::Time::TimeAbsolute oldest = now - KSystem::Time::TimeAbsolute::milliseconds(cleanupandbeacon * 4);

				while(hit != otherHosts.end())
				{
					if((*hit).first == Messaging::MessageEntry::HOST_ID_ANY_HOST) //Do not cleanup ANYHOST
					{
						++hit;
						continue;
					}

					if((*hit).second.lastseen < oldest)
					{
						//std::cout<<"Cleaning Host:"<<(*hit).first<<std::endl;
						t = hit++;
						otherHosts.erase(t);
					}
					else
						hit++;
				}

				cleanupLocalSubscriptions();


			}
			{
				HostSubscriptions *hs = HostSubscriptions::default_instance().New();
				hs->set_hostname(boost::asio::ip::host_name());
				std::map<hostid, hostDescription>::const_iterator hit = otherHosts.begin();

				for(; hit != otherHosts.end(); ++hit)
				{
					std::set<size_t>::const_iterator tit = (*hit).second.providesTopics.begin();

					for(; tit != (*hit).second.providesTopics.end(); ++tit)
					{
						Subscription *s = hs->add_topics();
						s->set_host((*hit).first);
						s->set_topicid(*tit);
						//std::cout<<"Requesting"<<(*hit).first<<" "<<(*tit)<<std::endl;
					}
				}

				Messaging::MessageEntry m;
				m.msg.reset(hs);
				m.topic = Messaging::Topics::Instance().getId("communication");
				m.msgclass = Messaging::MessageEntry::STATE;
				m.timestamp = now.wrapTo<KSystem::Time::TimeStamp>();
				m.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;
				//std::cout<<"Beacon"<<std::endl;
				processOutGoing(m);
			}
			{
				//Internal message, forwarded to localhost, keeps track of known hosts, and this host info.
				KnownHosts *kn = KnownHosts::default_instance().New();
				//Who am I?
				kn->mutable_localhost()->set_hostid(thishost);
				kn->mutable_localhost()->set_hostname(boost::asio::ip::host_name());

				std::map<hostid, hostDescription>::const_iterator kit = otherHosts.begin();

				for(; kit != otherHosts.end(); ++kit)
				{
					if((*kit).first == Messaging::MessageEntry::HOST_ID_ANY_HOST) //Do not report ANYHOST
						continue;

					HostEntry *e = kn->add_entrylist();
					e->set_hostid((*kit).first);
					e->set_hostname((*kit).second.hostname);
				}

				Messaging::MessageEntry m;
				m.msg.reset(kn);
				m.topic = Messaging::Topics::Instance().getId("communication");
				m.msgclass = Messaging::MessageEntry::STATE;
				m.timestamp = now.wrapTo<KSystem::Time::TimeStamp>();
				m.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;
				publish(m);
			}
			dep.cleanOlderThan(KSystem::Time::TimeAbsolute::milliseconds(cleanupandbeacon * 2));
			timer_.expires_from_now(boost::posix_time::milliseconds(cleanupandbeacon));
			timer_.async_wait(
			    boost::bind(&MulticastPoint::handle_timeout, this,
			                boost::asio::placeholders::error));
		}
	}

	int MulticastPoint::Execute()
	{
		rio.run_one();
	}

	packet MessageEntryToBytes(Messaging::MessageEntry const& m)
	{

	    packet p;
	    p.bytes=NULL;
	    p.size=0;

        MessageEntrySerializer<MessageEntrySerializationTraits> ser(m);
        std::size_t s=ser.getByteSize();
        if(s==0)
            return p;
        uint8_t *buff=new  uint8_t[s];
        p.bytes=buff;
        p.size=s;
        if(ser.writeSerialized(buff,s)!=s)
        {
            delete buff;
            p.size=0;
            p.bytes=NULL;

        }
        return p;


        /*char *r = new char[totalsize];
		size_t msgbytes = m.msg->ByteSize();
		size_t totalsize = msgbytes;
		totalsize += m.msg->GetTypeName().size();
		totalsize += sizeof(serializedmsgheader);
		char *r = new char[totalsize];
		char *d = r;
		serializedmsgheader *h = (serializedmsgheader *)d;
		h->encodeFromMsg(m, m.msg->GetTypeName().size());
		d += sizeof(serializedmsgheader);
		memcpy(d, m.msg->GetTypeName().data(), m.msg->GetTypeName().size());
		d += m.msg->GetTypeName().size();
		m.msg->SerializePartialToArray(d, msgbytes);
		packet p;
		p.bytes = r;
		p.size = totalsize;
		return p;*/
	}

	bool MessageEntryFromBytes(packet const& p, Messaging::MessageEntry &m)
	{
		/*const serializedmsgheader *h = (const serializedmsgheader*)p.bytes;
		m = h->decodeMsg();
		std::string TypeName = std::string(p.bytes + sizeof(serializedmsgheader), h->getTypeData());
		const google::protobuf::Descriptor *d = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(TypeName);

		if(!d)
		{
			std::cout << "Could Not Find Descriptor for:" << TypeName << std::endl;
			return false;
		}

		google::protobuf::Message *protomsg = google::protobuf::MessageFactory::generated_factory()->GetPrototype(d)->New();

		if( !protomsg->ParsePartialFromArray(
		            p.bytes + sizeof(serializedmsgheader) + h->getTypeData(),
		            p.size - (sizeof(serializedmsgheader) + h->getTypeData())
		        ) )
		{
			std::cout << "Cannot Parse Message of type :" << TypeName << std::endl;
			return false;
		}

		m.msg.reset(protomsg);
		//std::cout<<m.msg.get()<<std::endl;
		m.host = Messaging::MessageEntry::HOST_ID_LOCAL_HOST;
		return true;*/
	}

	void MulticastPoint::bufferCallback(Messaging::MessageBuffer *mbuf)
	{
		std::vector<Messaging::MessageEntry> v = remove();

		for(unsigned i = 0; i < v.size(); i++)
		{
			rio.post(boost::bind(&MulticastPoint::processOutGoing, this, v[i]) );
		}
	}
	void MulticastPoint::processOutGoing(MessageEntry m)
	{
		if(m.msgclass >= MessageEntry::SUBSCRIBE_ON_TOPIC && m.msgclass <= MessageEntry::UNSUBSCRIBE_ALL_TOPIC)
		{
			bool actionadd = m.msgclass >= MessageEntry::SUBSCRIBE_ON_TOPIC && m.msgclass <= MessageEntry::SUBSCRIBE_ALL_TOPIC;

			if(m.host == MessageEntry::HOST_ID_LOCAL_HOST || m.host == thishost)
				return;

			//Unkown Host, reject

			if(m.host != MessageEntry::HOST_ID_ANY_HOST && otherHosts.find(m.host) == otherHosts.end())
				return;

			std::set<std::size_t> topics = Topics::Instance().iterateTopics(m.topic, m.msgclass);

			if(actionadd)
				otherHosts[m.host].providesTopics.insert(topics.begin(), topics.end());
			else if(m.host==MessageEntry::HOST_ID_ANY_HOST) //Unubscribe from ALL hosts
			{

				std::set<size_t>::iterator sit;

				for(sit = topics.begin(); sit != topics.end(); ++sit)
				{
					std::map<hostid, hostDescription>::iterator  hit = otherHosts.begin();
					for( hit = otherHosts.begin(); hit != otherHosts.end(); hit++)
						(*hit).second.providesTopics.erase(*sit);

				}




			}
			else //Unsubscribe from host
			{
				std::set<size_t>::iterator sit;

				for(sit = topics.begin(); sit != topics.end(); ++sit)
					otherHosts[m.host].providesTopics.erase(*sit);

				//std::cout << m.host << " unsubscribed from " << (*sit) << std::endl;
			}

			return;
		}

		if(m.msg == NULL)
			return;

		//Very early reject
		if(maxth <= queuesize)
		{
			std::cout << "Congestion on multicast" << std::endl;
			return ;
		}

		//std::cout<<"avgqueue:"<<avgqueuesize<<std::endl;
		//std::cout<<"checking:"<<m.msg->GetTypeName()<<std::endl;
		if(minth < avgqueuesize)
		{
			double pb = (( (double) (avgqueuesize - minth) )/ (maxth - minth))* ceil( ( (double)(m.msg->ByteSize()) ) / MAX_UDP_PAYLOAD) ;
			pb = sqrt(pb);

			//pb+=(size/(1024.0f*MAX_UDP_PAYLOAD))*(size/(4096.0f*MAX_UDP_PAYLOAD));
			//std::cout<<"pb:"<<pb<<std::endl;
			if(uni() < pb)
			{
				//std::cout<<"Rejecting:"<<m.msg->GetTypeName()<<std::endl;
				return ;
			}
		}

		packet pack = MessageEntryToBytes(m); //Serialize MessageEntry
		//std::cout<<"New set"<<std::endl;
		{
			SystemMutex::scoped_lock data_lock(mut);
			queuesize++;
		}
		//std::cout<<"Pending:"<<queuesize<<std::endl;
		sio.post(boost::bind(&MulticastPoint::handle_send_to, this, pack.bytes, pack.size) );
		//multisend.async_send_to(
		//   boost::asio::buffer(b.data[i].bytes,b.data[i].size), multicast_point_);
		//queue_receive();
		//              boost::bind(&MulticastPoint::handle_send_to, this,
		//            boost::asio::placeholders::error));
		//Just a little bit of headroom
	}

	void MulticastPoint::processIncoming( const uint8_t * buff, size_t size)
	{
		//Gives possetion of aquired  buffer to Depacketizer
		RawDepacketizer::depacketizer_result  r = dep.feed(buff, size);

		if(r.host <= 1)
		{
			if(r.p.bytes != NULL)
				delete[] r.p.bytes;

			return;
		}

		//std::cout<<"host:"<<r.host<<std::endl;
		MessageEntry m;

		if(!MessageEntryFromBytes(r.p, m))
		{
			if(r.p.bytes != NULL)
				delete[] r.p.bytes;

			return;
		}

		delete[] r.p.bytes;

		//Get timestamp;
		KSystem::Time::TimeAbsolute now =KSystem::Time::SystemTime::now();

		if(m.msg->GetTypeName() == "HostSubscriptions" && r.host != thishost)
		{
			hostDescription &hd = otherHosts[r.host];
			hd.lastseen = now;
			hd.timecorrection = hd.lastseen - now.unwrap(m.timestamp);
			//std::cout<<"Found host:"<<r.host<<std::endl;
			std::vector<MessageEntry> newsubscriptions;
			MessageEntry kh;
			kh.msgclass = MessageEntry::SUBSCRIBE_ON_TOPIC;
			kh.host = MessageEntry::HOST_ID_LOCAL_HOST;
			boost::shared_ptr<const HostSubscriptions> hs = boost::static_pointer_cast<const HostSubscriptions>(m.msg);
			hd.hostname = hs->hostname(); //Get HostName from the remote host
			const ::google::protobuf::RepeatedPtrField< ::Subscription >& fptr = hs->topics();
			::google::protobuf::RepeatedPtrField< ::Subscription >::const_iterator cit;
			hd.needsTopics.clear();
			for(cit = fptr.begin(); cit != fptr.end(); ++cit)
			{
				if((*cit).host() == thishost || (*cit).host() == MessageEntry::HOST_ID_ANY_HOST)
				{
					hd.needsTopics.insert((*cit).topicid());

					if(localsubscriptions.find((*cit).topicid()) != localsubscriptions.end()) //Already subscribed f0r s0ke
						continue;

					//std::cout<<"Host:"<<r.host<<" needs topic:"<<(*cit).topicid()<<std::endl;
					kh.topic = (*cit).topicid();
					localsubscriptions.insert((*cit).topicid());
					newsubscriptions.push_back(kh);
				}
			}

			publish(newsubscriptions);//Ask MessageQueue for these new subscriptions
			cleanupLocalSubscriptions();
		}

		if(m.msg->GetTypeName() == "KnownHosts") //Who is that guy trying to pollute my host?
			return ;

		if(otherHosts.find(r.host) == otherHosts.end()) //Unknown Host, reject
			return;

		//std::cout<<"Host:"<<r.host<<","<<m.topic<<std::endl;

		if(otherHosts[r.host].providesTopics.find(m.topic) ==
		        otherHosts[r.host].providesTopics.end() //Subscription to this host
		        &&
		        otherHosts[anyhost].providesTopics.find(m.topic) ==
		        otherHosts[anyhost].providesTopics.end()//Or to any host
		  )
			return;

		m.timestamp = (now + (otherHosts[r.host]).timecorrection).wrapTo<KSystem::Time::TimeStamp>(); //Correct timestamp
		m.host = r.host; //Fix Host
		//std::cout<<"Received:"<<m.msg->GetTypeName()<<std::endl;
		publish(m);
	}

}
