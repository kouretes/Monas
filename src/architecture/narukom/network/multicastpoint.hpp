#ifndef MULTICASTPOINT_H
#define MULTICASTPOINT_H
#include <string>

#include "architecture/narukom/pub_sub/endpoint.h"
#include "hal/thread.h"
#include "hal/mutex.h"
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "rawPacketDePacket.h"


#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

#include <set>


namespace KNetwork{

class MulticastPoint : public EndPoint, private KSystem::Thread
{
	public:
	MulticastPoint(std::string const& name,unsigned payloadsize);
	~MulticastPoint();
	int startEndPoint(std::string const& multicast_ip,unsigned int port);
	void setCleanupAndBeacon(unsigned i);
	private:
	int Execute();
	void handle_receive_from(const char *buffer,const boost::system::error_code& error,size_t bytes_recvd);

	void handle_send_to(const char *buffer,std::size_t size);//Executed by sio service in sendthread context;
	void handle_timeout(const boost::system::error_code& error);

	void queue_receive();
	hostid thishost;

	boost::asio::io_service rio,sio;
	boost::asio::io_service::work senderwork;//To keep sendthread running
	boost::thread sendthread;
	boost::asio::ip::udp::socket multireceive,multisend;
	unsigned multiport;
	boost::asio::ip::udp::endpoint sender_endpoint_,multicast_point_;
	boost::asio::deadline_timer timer_;

	unsigned payloadsize_;

	RawDepacketizer dep;
	RawPacketizer   p;
	unsigned cleanupandbeacon;
	bool canWarn; //Used for emmitting send error warnings

	struct hostDescription{
		//hostid h;
		std::string hostname;
		boost::posix_time::time_duration timecorrection;
		boost::posix_time::ptime lastseen;
		std::set<size_t> needsTopics;//Ie what I broadcast because this host needs it
		std::set<size_t> providesTopics;//Ie what I receive from this host
		/*bool operator== (const struct hostDescription & b) const {return h==b.h;	};
		bool operator<(const struct hostDescription &b) const {return h < b.h;};*/

	};


	std::map<hostid,hostDescription> otherHosts;
	std::set<size_t>   localsubscriptions;//Ie what I ask from messagequeue currently

	void  bufferCallback(MessageBuffer *mbuf);
	void processIncoming(const char * buff, size_t size);
	void processOutGoing(msgentry m);


	KSystem::Mutex  mut;


	//Random Early Detection... for outgoing only

	unsigned queuesize;
	float avgqueuesize;

	boost::variate_generator<boost::mt19937, boost::uniform_real<> > uni; //Randomness

	static const float wq=0.75;

	static const std::size_t minth=16;
	static const std::size_t maxth=48;




};

}



#endif /*MULTICASTPOINT_H*/
