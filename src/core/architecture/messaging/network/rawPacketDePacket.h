#ifndef RAWPACKETDEPACKET_H
#define RAWPACKETDEPACKET_H
#include "core/architecture/time/TimeTypes.hpp"
#include <vector>
#include <set>
#include <map>
#include <string>
//#include "hal/mutex.h"


namespace KNetwork
{
	static const unsigned MAX_UDP_PAYLOAD = 1280;
	typedef unsigned char msgid;
	typedef uint16_t sequenceid;
	typedef uint32_t hostid;

	static const unsigned char PACKETFLAG_FRISTPACKET = 1;

#pragma pack(push,1) //Force continuity
	struct packetheader
	{
		hostid hid;
		msgid mid;//8bits
		unsigned char flags;
		sequenceid number;

	};
#pragma pack(pop)
	struct packet
	{
		int size;
		const uint8_t *bytes;
	};

	class RawPacketizer
	{
	public:
		RawPacketizer();
		void setHost(hostid h);
		void assign(const uint8_t* bytes, std::size_t size);

		std::size_t  nextPacket();

		uint8_t buff[MAX_UDP_PAYLOAD];
	private:
		msgid nextmid;
		packetheader hdr;
		size_t numofpackets;
		std::size_t currentpacket;
		const uint8_t * nextbyte;
		const uint8_t* srcbytes;
		std::size_t srcsize;
		size_t payloadlength;

	};

	class RawDepacketizer
	{
	public:

		struct depacketizer_result
		{
			hostid host;
			packet p;
		};
		void setHost(hostid h);
		depacketizer_result feed(const uint8_t *const bytes, std::size_t size);
		//depacketizer_result feed(std::string const& s);
		void * getbuffer();
		void releaseBuffer(const void * buf);
		std::size_t getbufferSize() const;
		void cleanOlderThan(KSystem::Time::TimeDuration td);

	private:

		class partialMessage
		{
		public:
			partialMessage();
			~partialMessage();
			std::map<sequenceid, packet> packets;
			std::size_t totalpackets;
			KSystem::Time::TimeAbsolute lastupdate;

		};
		typedef std::map<msgid, partialMessage> hostMessages;

		hostid thisid;
		std::map<hostid, hostMessages> pending;

	};



}




#endif /*RAWPACKET_H */
