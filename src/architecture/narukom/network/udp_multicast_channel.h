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

#ifndef UDPMULCASTCHANNEL_H
#define UDPMULCASTCHANNEL_H

#include "Thread.h"
#include "subscriber.h"
#include "publisher.h"
#include <boost/asio.hpp>
#include "network_message_buffer.h"
#include "network_channel.h"
#include "multicastsender_receiver.h"

class NackMessage;
class UdpMulticastChannel :  public NetworkChannel
{
  public:
    
    
    UdpMulticastChannel(const UdpMulticastChannel& );
    UdpMulticastChannel(const std::string& multicast_address = "224.0.0.1" , int port = 19786, bool running = false ) : NetworkChannel(false,"udp_multicast")
    {
			incoming = new NetworkMessageBuffer(this);
			outgoing = new NetworkMessageBuffer(this);
			io_service = new boost::asio::io_service;
			multicast_addr = new boost::asio::ip::address(boost::asio::ip::address::from_string(multicast_address));
			sock = new boost::asio::ip::udp::socket(*io_service);
			receiver = new MulticasReceiver(this,io_service,sock,multicast_addr,multicast_addr,port);
			sender = new MulticastSender(this,io_service,multicast_addr,sock,port);
			last_send[0] = 0;
			last_send[1] = 0;
		
		}
    virtual void run();
    virtual void process_messages();
		virtual void publish(google::protobuf::Message*);
		virtual void get_network_messages(){}
		void deliver(NetworkMessage*,NetworkMessageBuffer * buf);
		void notify_sent(unsigned int packet_num, unsigned int message_num);
		void notify_sent_timeout(unsigned int packet_num, unsigned int message_num);
		void notify_rec_timeout();
		void notify_received(char* data, size_t sz,boost::asio::ip::udp::endpoint sender_endpoint);
		void send_nack(NetworkPacket* packet,boost::asio::ip::udp::endpoint& endpoint, std::vector<unsigned>*);
		void respond_nack(const NackMessage&);
		virtual void write_to_network();
		virtual void read_from_network();
		virtual void receive_from_network();
//    virtual ~Publisher();
  private:
		void network_to_google_message(NetworkMessage* network_message);
		boost::asio::io_service* io_service;
		boost::asio::ip::address* multicast_addr;// = boost::asio::ip::address::from_string("224.0.0.1");
		boost::asio::ip::udp::socket* sock;
		MulticasReceiver* receiver;
		MulticastSender* sender;
		NetworkMessageBuffer* incoming;
		NetworkMessageBuffer* outgoing;
		unsigned last_send[2];
		std::vector<std::pair<unsigned int, unsigned int> > timed_out;
		
//   boost::asio::ip::multicast::join_group option(m_addr);
  /*udp::socket sock(io_service);
  sock.open(udp::v4());
  sock.set_option(option);*/
    
};

#endif // UDPMULCASTPUBSUB_H
