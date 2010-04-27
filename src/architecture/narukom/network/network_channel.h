/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef NETWORK_CHANNEL_H
#define NETWORK_CHANNEL_H
#include "network_message.h"
#include "publisher.h"
#include "subscriber.h"
#include "Mutex.h"
// #include "network_message_buffer.h"
#include <google/protobuf/message.h>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/ip/udp.hpp>
class NetworkMessageBuffer;

class NetworkChannel : public Thread, public Subscriber, public Publisher
{
	public:
		std::string get_host_name(){return boost::asio::ip::host_name();}
		NetworkChannel(bool start_run = false, std::string name = "Default NetworkChannel") : Thread(false),Subscriber(name),Publisher(name),current_message(0){}
		virtual void deliver(NetworkMessage*,NetworkMessageBuffer * buf);
		virtual void notify_sent(unsigned int packet_num, unsigned int message_num){
			std::cout << "packet: " << packet_num << " of " << message_num << " sendt " << std::endl;
		}
		virtual void notify_sent_timeout(unsigned int packet_num, unsigned int message_num){
			std::cout << " sent timeout packet:  " << packet_num << " of " << message_num << std::endl;
		
		}
		virtual void notify_rec_timeout()
		{
			std::cout << "receive timeout " << std::endl;
		}
		virtual void notify_received(char* data, size_t sz,boost::asio::ip::udp::endpoint sender_endpoint)
		{
		
			std::cout << "Received: " << sz << " bytes " << std::endl;
		}
		unsigned int get_next_message_number(){
		// mx.Lock();
		 current_message++;
		 //mx.Unlock();
		 return current_message;
		}
			private:
				unsigned int current_message;
				Mutex mx;
};

#endif // NETWORK_CHANNEL_H
