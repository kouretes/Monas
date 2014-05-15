#ifndef MULTICASTPOINT_HPP
#define MULTICASTPOINT_HPP
#include <string>

#include "core/architecture/messaging/EndPoint.hpp"
#include "hal/SystemThread.hpp"
#include "hal/SystemMutex.hpp"
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "rawPacketDePacket.h"


#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

#include <set>


namespace KNetwork
{

	class MulticastPoint : public EndPoint, private KSystem::SystemThread
	{
	public:
		MulticastPoint(std::string const& name, unsigned payloadsize);
		~MulticastPoint();
		bool startEndPoint(std::string const& multicast_ip, unsigned int port);
		void setCleanupAndBeacon(unsigned i);
	private:
		int Execute();
		void handle_receive_from(const char *buffer, const boost::system::error_code& error, size_t bytes_recvd);

		void handle_send_to(const char *buffer, std::size_t size); //Executed by sio service in sendthread context;
		void handle_timeout(const boost::system::error_code& error);

		void queue_receive();
		hostid thishost;

		boost::asio::io_service rio, sio;
		boost::asio::io_service::work senderwork;//To keep sendthread running
		boost::thread sendthread;
		boost::asio::ip::udp::socket multireceive, multisend;
		unsigned multiport;
		boost::asio::ip::udp::endpoint sender_endpoint_, multicast_point_;
		boost::asio::deadline_timer timer_;

		unsigned payloadsize_;

		RawDepacketizer dep;
		RawPacketizer   p;
		unsigned cleanupandbeacon;
		bool canWarn; //Used for emmitting send error warnings

		struct hostDescription
		{
			//hostid h;
			std::string hostname;
			KSystem::Time::TimeDuration timecorrection;
			KSystem::Time::TimeAbsolute lastseen;
			std::set<size_t> needsTopics;//Ie what I broadcast because this host needs it
			std::set<size_t> providesTopics;//Ie what I receive from this host
			/*bool operator== (const struct hostDescription & b) const {return h==b.h;	};
			bool operator<(const struct hostDescription &b) const {return h < b.h;};*/

		};


		std::map<hostid, hostDescription> otherHosts;
		std::set<size_t>   localsubscriptions;//Ie what I ask from messagequeue currently

		void  bufferCallback(MessageBuffer *mbuf);
		void processIncoming(const char * buff, size_t size);
		void processOutGoing(msgentry m);

		void cleanupLocalSubscriptions();


		KSystem::SystemMutex  mut;


		//Random Early Detection... for outgoing only

		unsigned queuesize;
		float avgqueuesize;

		boost::variate_generator<boost::mt19937, boost::uniform_real<> > uni; //Randomness

		static const float wq = 0.75;

		static const std::size_t minth = 16;
		static const std::size_t maxth = 48;




	};

}



#endif /*MULTICASTPOINT_H*/
