
#include "rawPacketDePacket.h"
#include "core/architecture/time/SystemTime.hpp"
// Required for hton*
#include  <boost/asio.hpp>
#include <vector>
using namespace std;
namespace KNetwork
{

	class qnd_pool
	{
	public:
		qnd_pool(std::size_t t): chunksize(t), freechucks()
		{
		}
		void *malloc()
		{
			void * t;

			if(freechucks.size() > 0)
			{
				t  = freechucks.back();
				freechucks.pop_back();
			}
			else t = system_alloc();

			//std::cout<<"malloc:"<<((int)t)<<std::endl;
			return t;
		}
		void free(void * t)
		{
			//std::cout<<"free:"<<((int)t)<<std::endl;
			freechucks.push_back(t);
		}
		std::size_t getChunksize()
		{
			return chunksize;
		}
	private:
		void *system_alloc()
		{
			return new char[chunksize];
		};



		std::size_t chunksize;
		std::vector<void *> freechucks;

	};

	qnd_pool bytepool(MAX_UDP_PAYLOAD);

	RawPacketizer::RawPacketizer(): nextmid(0)
	{
	};

	void RawPacketizer::setHost(hostid h)
	{
		hdr.hid = htonl(h);
	};


	void RawPacketizer::assign(const uint8_t* bytes, std::size_t size)
	{
		nextmid++;
		numofpackets = ceil( ((float)size) / (MAX_UDP_PAYLOAD - sizeof(packetheader)) );
		nextbyte = bytes;
		srcbytes = bytes;
		srcsize = size;
		currentpacket = 0;
		payloadlength = ceil( ((float)size) / (numofpackets) ); //Remainder is accounted for  at the last packet
		//std::cout<<payloadlength<<std::endl;
	}


	std::size_t RawPacketizer::nextPacket()
	{
		size_t packetsize = payloadlength + sizeof(packetheader);

		if(currentpacket >= numofpackets)  return 0; //Done

		if(currentpacket == numofpackets - 1) //lastpacket
		{
			//std::cout<<"Size:"<<srcsize<<" "<<(nextbyte-srcbytes)<<std::endl;
			payloadlength = srcsize - (nextbyte - srcbytes); //Remaining bytes;
			packetsize = payloadlength + sizeof(packetheader);
			// std::cout<<"last:"<<packetsize<<std::endl;
		}

		// std::cout<<"Size:"<<packetsize<<std::endl;
		hdr.number = htons(numofpackets - currentpacket - 1); //<---------------- networkbyteorder
		hdr.mid = nextmid;
		hdr.flags = (currentpacket == 0) ? PACKETFLAG_FRISTPACKET : 0;
		uint8_t *destpointer = buff;
		memcpy(destpointer, &hdr, sizeof(packetheader));
		destpointer += sizeof(packetheader); //Advance to data;
		memcpy(destpointer, nextbyte, payloadlength);
		nextbyte += payloadlength;
		currentpacket++;
		return packetsize;
	}

	void RawDepacketizer::setHost(hostid h)
	{
		thisid = h;
	};

	RawDepacketizer::partialMessage::partialMessage(): totalpackets(0)
	{
	};

	RawDepacketizer::partialMessage::~partialMessage()
	{
		std::map<sequenceid, packet>::iterator pit = packets.begin();

		while(pit != packets.end())
		{
			//std::cout<<"Free partial:"<<std::endl;
			bytepool.free(const_cast<void *>((void *)(*pit).second.bytes));
			pit++;
		};
	}

	/*RawDepacketizer::depacketizer_result RawDepacketizer::feed(std::string const& s)
	{
		return feed(s.data(), s.size());
	};*/

	RawDepacketizer::depacketizer_result RawDepacketizer::feed(const uint8_t *const bytes, std::size_t size)
	{
		static const int hsize = sizeof(packetheader);
		packetheader ph = *((const packetheader *)bytes);
		//==== REorder bytes
		ph.hid = ntohl(ph.hid);
		ph.number = ntohs(ph.number);
		depacketizer_result res;
		res.host = 0;
		res.p.bytes = 0;
		res.p.size = 0;

		if(ph.hid == thisid)
		{
			bytepool.free(const_cast<void *>((void *)bytes));
			return res;
		}

		partialMessage &pm = pending[ph.hid][ph.mid];
		pm.packets[ph.number].bytes = bytes;
		pm.packets[ph.number].size = size;

		if(ph.flags & PACKETFLAG_FRISTPACKET)
			pm.totalpackets = ph.number + 1;

		pm.lastupdate = KSystem::Time::SystemTime::now();

		//if(ph.number==0)
		//std::cout<<"Host:"<<(ph.hid)<<" messageid:"<<(int)(ph.mid)<<" Number:"<<(ph.number)
		//    <<"Needmore:"<<(pm.totalpackets-pm.packets.size())<<" flag:"<<(int)(ph.flags)<<std::endl;
		if(pm.packets.size() == pm.totalpackets)
		{
			res.host = ph.hid;
			size_t reservesize = 0;

			for(sequenceid i = 0; i < pm.totalpackets; i++)
				reservesize += pm.packets[pm.totalpackets - i - 1].size - hsize;

			res.p.size = reservesize;
			uint8_t *p = new uint8_t[reservesize];
			res.p.bytes = p;

			for(sequenceid i = 0; i < pm.totalpackets; i++)
			{
				memcpy(p, (uint8_t *)pm.packets[pm.totalpackets - i - 1].bytes + hsize, pm.packets[pm.totalpackets - i - 1].size - hsize);
				p += pm.packets[pm.totalpackets - i - 1].size - hsize;
			}

			pending[ph.hid].erase(ph.mid);
		}

		return res;
	};


	void * RawDepacketizer::getbuffer()
	{
		void *buf = bytepool.malloc();
		//std::cout<<"malloc:"<<((int)buf)<<std::endl;
		return buf;
	}
	void  RawDepacketizer::releaseBuffer(const void * buf)
	{
		//std::cout<<"Free release:"<<std::endl;
		bytepool.free(const_cast<void *>(buf));
	}


	size_t RawDepacketizer::getbufferSize() const
	{
		return bytepool.getChunksize();
	}


	void RawDepacketizer::cleanOlderThan(KSystem::Time::TimeDuration t)
	{
		KSystem::Time::TimeAbsolute now = KSystem::Time::SystemTime::now();
		std::map<hostid, hostMessages>::iterator hit = pending.begin();

		for(; hit != pending.end(); hit++)
		{
			hostMessages::iterator mit = (*hit).second.begin();

			for(; mit != (*hit).second.end();)
			{
				if(now - (*mit).second.lastupdate > t)
				{
					hostMessages::iterator t = mit++;
					(*hit).second.erase(t);
					//sigit=sigdata.begin();
				}
				else
					++mit;
			}
		}
	};


}

