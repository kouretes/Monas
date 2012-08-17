#ifndef RAWPACKET_H
#define RAWPACKET_H
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <set>
#include <map>
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
		const char *bytes;
	};

	class RawPacketizer
	{
	public:
		RawPacketizer();
		void setHost(hostid h);
		void assign(const char* bytes, std::size_t size);

		std::size_t  nextPacket();

		char buff[MAX_UDP_PAYLOAD];
	private:
		msgid nextmid;
		packetheader hdr;
		size_t numofpackets;
		std::size_t currentpacket;
		const char * nextbyte;
		const char* srcbytes;
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
		depacketizer_result feed(const char *const bytes, std::size_t size);
		depacketizer_result feed(std::string const& s);
		void * getbuffer();
		void releaseBuffer(const void * buf);
		std::size_t getbufferSize() const;
		void cleanOlderThan(boost::posix_time::time_duration td);

	private:

		class partialMessage
		{
		public:
			partialMessage();
			~partialMessage();
			std::map<sequenceid, packet> packets;
			std::size_t totalpackets;
			boost::posix_time::ptime lastupdate;

		};
		typedef std::map<msgid, partialMessage> hostMessages;

		hostid thisid;
		std::map<hostid, hostMessages> pending;

	};



}




#endif /*RAWPACKET_H */
