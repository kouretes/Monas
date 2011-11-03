#ifndef RAWPACKET_H
#define RAWPACKET_H
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <set>
#include <map>
//#include "hal/mutex.h"


namespace KNetwork
{
	static const unsigned MAX_UDP_PAYLOAD=512;
	typedef unsigned char msgid;
	typedef uint16_t sequenceid;
	typedef uint32_t hostid;

	static const unsigned char PACKETFLAG_FRISTPACKET=1;

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

	class RawPacketBatch
	{
		public:
		RawPacketBatch(size_t i):data(i) {};
		std::vector<packet> data;
		~RawPacketBatch() ;

	};

  class RawPacketizer
  {
    public:
	RawPacketizer();
	void setHost(hostid h);
    RawPacketBatch feed(const  char *const bytes,std::size_t size);
    RawPacketBatch feed(std::string const& s) ;
    private:
    msgid nextmid;
    packetheader hdr;
  };

  class RawDepacketizer
  {
    public:

	struct depacketizer_result
	{
		hostid host;
		packet p;
	};
    depacketizer_result feed(const char *const bytes,std::size_t size);
    depacketizer_result feed(std::string const& s);
    void * getbuffer() const;
    void releaseBuffer(const void * buf) const;
	std::size_t getbufferSize() const;

    private:

	class partialMessage
	{
		public:
		partialMessage();
		~partialMessage();
		std::map<sequenceid,packet> packets;
		std::size_t totalpackets;
		boost::posix_time::ptime lastupdate;

	};
	typedef std::map<msgid,partialMessage> hostMessages;


    std::map<hostid,hostMessages> pending;
    void cleanOlderThan(boost::posix_time::time_duration td);
  };



}




#endif /*RAWPACKET_H */
