#ifndef MULTICASTPOINT_H
#define MULTICASTPOINT_H
#include <string>

#include "architecture/narukom/pub_sub/endpoint.h"
#include "hal/thread.h"
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "rawPacketDePacket.h"

#include <set>


namespace KNetwork{

class MulticastPoint : public EndPoint, private Thread
{
	public:
	MulticastPoint(std::string const& name,unsigned payloadsize);
	~MulticastPoint();
	int startEndPoint(std::string const& multicast_ip,unsigned int port);
	void setCleanupAndBeacon(unsigned i);
	private:
	int Execute();
	void handle_receive_from(const char *buffer,const boost::system::error_code& error,size_t bytes_recvd);

	void handle_send_to(const boost::system::error_code& error);
	void handle_timeout(const boost::system::error_code& error);

	void queue_receive();
	hostid thishost;

	boost::asio::io_service io;
	boost::asio::ip::udp::socket multisocket;
	unsigned localport,multiport;
	boost::asio::ip::udp::endpoint sender_endpoint_,multicast_point_;
	boost::asio::deadline_timer timer_;

	unsigned payloadsize_;

	RawDepacketizer dep;
	RawPacketizer   p;
	unsigned cleanupandbeacon;

	struct hostDescription{
		hostid h;
		boost::posix_time::time_duration timecorrection;
		boost::posix_time::ptime lastseen;
		bool operator== (const struct hostDescription & b) const {return h==b.h;	};
		bool operator<(const struct hostDescription &b) const {return h < b.h;};

	};


	std::set<hostDescription> otherHosts;
	std::set<size_t>   localsubscriptions;//Ie what I ask from messagequeue
	std::set< std::pair<hostid,std::size_t> > remoteSubscriptions;//Ie what I broadcast

	void  bufferCallback(MessageBuffer *mbuf);
	void processIncoming(const char * buff, size_t size);
	void processOutGoing(msgentry m);


};

}



#endif /*MULTICASTPOINT_H*/
