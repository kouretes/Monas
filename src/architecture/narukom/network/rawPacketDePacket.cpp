
#include "rawPacketDePacket.h"
#include  <boost/asio.hpp>
#include <boost/pool/pool.hpp>

using namespace std;
namespace KNetwork
{
boost::pool<> bytepool(MAX_UDP_PAYLOAD);
RawPacketBatch::~RawPacketBatch()
{
	for(unsigned i=0;i<data.size();i++)
	{
		if(data[i].bytes!=NULL)
			bytepool.free(const_cast<void *>((const void *)data[i].bytes));
	}
};

RawPacketizer::RawPacketizer():nextmid(0) {};

void RawPacketizer::setHost(hostid h)  { hdr.hid=htonl(h);	};


RawPacketBatch RawPacketizer::feed(const  char *const bytes,std::size_t size)
{

	size_t numofpackets= ceil( ((float)size)/(MAX_UDP_PAYLOAD-sizeof(packetheader)) );
	size_t processedbytes=0;
	const  char *srcpointer=bytes;
	char *destpointer=0;

	size_t payloadlength=size/numofpackets; //Remainder is accounted for  at the last packet
	size_t packetsize=payloadlength+sizeof(packetheader);


	RawPacketBatch res(numofpackets);
	for(size_t i=0;i<numofpackets;i++)
	{
		if(i==numofpackets-1)//lastpacket
		{
			payloadlength=size-processedbytes;
			packetsize=payloadlength+sizeof(packetheader);
		}

		res.data[i].bytes=destpointer=(char *)bytepool.malloc();
		res.data[i].size=packetsize;

		//Fix header
		hdr.number=htons(numofpackets-i-1);//<---------------- networkbyteorder
		hdr.mid=nextmid;
		hdr.flags= (i==0)?PACKETFLAG_FRISTPACKET:0;
		//Copy it
		memcpy(destpointer,&hdr,sizeof(packetheader));
		destpointer+=sizeof(packetheader); //Advance to data;

		memcpy(destpointer,srcpointer,payloadlength);
		//Move along data
		processedbytes+=payloadlength;
		srcpointer+=payloadlength;

	}

	nextmid++;
	return res;
}


RawDepacketizer::partialMessage::partialMessage(): totalpackets(0)
{

};

RawDepacketizer::partialMessage::~partialMessage()
{

	std::map<sequenceid,packet>::iterator pit=packets.begin();
	while(pit!=packets.end())
	{

		bytepool.free(const_cast<void *>((void *)(*pit).second.bytes));
		pit++;
	};
}

RawDepacketizer::depacketizer_result RawDepacketizer::feed(std::string const& s) { return feed(s.data(),s.size());};

RawDepacketizer::depacketizer_result RawDepacketizer::feed(const char *const bytes,std::size_t size)
{
	static const int hsize=sizeof(packetheader);

	packetheader ph=*((const packetheader *)bytes);
	//==== REorder bytes
	ph.hid=ntohl(ph.hid);
	ph.number=ntohs(ph.number);

	//std::cout<<"Host:"<<(ph.hid)<<" messageid:"<<(int)(ph.mid)<<" Number:"<<(ph.number)<<" flag:"<<(int)(ph.flags)<<std::endl;
	partialMessage &pm=pending[ph.hid][ph.mid];
	pm.packets[ph.number].bytes=bytes;
	pm.packets[ph.number].size=size;
	if(ph.flags&PACKETFLAG_FRISTPACKET)
		pm.totalpackets=ph.number+1;
	pm.lastupdate=boost::posix_time::microsec_clock::universal_time();
	depacketizer_result res;
	res.host=0;
	res.p.bytes=0;
	res.p.size=0;
	if(pm.packets.size()==pm.totalpackets)
	{
		res.host=ph.hid;
		size_t reservesize=0;
		for(sequenceid i=0;i<pm.totalpackets;i++)
			reservesize+=pm.packets[pm.totalpackets-i-1].size-hsize;
		res.p.size=reservesize;
		char *p=new char[reservesize];
		res.p.bytes=p;
		for(sequenceid i=0;i<pm.totalpackets;i++)
		{
			memcpy(p,(char *)pm.packets[pm.totalpackets-i-1].bytes+hsize,pm.packets[pm.totalpackets-i-1].size-hsize);
			p+=pm.packets[pm.totalpackets-i-1].size-hsize;
		}
		pending[ph.hid].erase(ph.mid);
	}
	return res;

};


void * RawDepacketizer::getbuffer() const
{
	return bytepool.malloc();
}
void  RawDepacketizer::releaseBuffer(const void * buf) const
{
	return bytepool.free(const_cast<void *>(buf));
}


size_t RawDepacketizer::getbufferSize() const
{
	return bytepool.get_requested_size();
}


void RawDepacketizer::cleanOlderThan(boost::posix_time::time_duration t)
{
	boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();

	std::map<hostid,hostMessages>::iterator hit= pending.begin();
	for(;hit!=pending.end();hit++)
	{
		hostMessages::iterator mit=(*hit).second.begin();
		for(;mit!=(*hit).second.end();)
		{
			if(now-(*mit).second.lastupdate >t)
			{
				hostMessages::iterator t=mit++;
				(*hit).second.erase(t);
				//sigit=sigdata.begin();
			}
			else
				++mit;

		}
	}
};


}

