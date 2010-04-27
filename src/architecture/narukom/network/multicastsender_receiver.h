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

#ifndef MULTICASTSENDER_RECEIVER_H
#define MULTICASTSENDER_RECEIVER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "network_channel.h"
#include "network_common.h"
// const short multicast_port = 30001;


class MulticastSender
{
    public:
        MulticastSender(NetworkChannel* owner,boost::asio::io_service* io_service,
                const boost::asio::ip::address* multicast_address,
                boost::asio::ip::udp::socket* sock ,short multicast_port)
            : owner(owner),
            endpoint_(*multicast_address, multicast_port),
            timer_(*io_service),
            multicast_port(multicast_port)
    {
        if(sock == 0) {
            socket_ = new  boost::asio::ip::udp::socket(*io_service,endpoint_.protocol());
        }
        else  {
            socket_ = sock;
            if(!socket_->is_open())
                socket_->open(endpoint_.protocol());
        }
    }


        void async_send(const std::string& msg, unsigned int num_of_packet, unsigned int message_num,boost::asio::ip::udp::endpoint* endpoint =0){
            if(endpoint==0)
                socket_->async_send_to(
                        boost::asio::buffer(msg), endpoint_,
                        boost::bind(&MulticastSender::handle_send_to, this,
                            boost::asio::placeholders::error,num_of_packet,message_num));
            else
                socket_->async_send_to(
                        boost::asio::buffer(msg), *endpoint,
                        boost::bind(&MulticastSender::handle_send_to, this,
                            boost::asio::placeholders::error,num_of_packet,message_num));

            timer_.expires_from_now(boost::posix_time::millisec(50));
            timer_.async_wait(
                    boost::bind(&MulticastSender::handle_timeout, this,
                        boost::asio::placeholders::error,num_of_packet,message_num));
        }

        void async_send(const char* msg, unsigned int size, unsigned int num_of_packet, unsigned int message_num,boost::asio::ip::udp::endpoint* endpoint = 0){
            if(endpoint == 0)
                socket_->async_send_to(
                        boost::asio::buffer(msg,size), endpoint_,
                        boost::bind(&MulticastSender::handle_send_to, this,
                            boost::asio::placeholders::error,num_of_packet,message_num));
            else
                socket_->async_send_to(
                        boost::asio::buffer(msg,size), *endpoint,
                        boost::bind(&MulticastSender::handle_send_to, this,
                            boost::asio::placeholders::error,num_of_packet,message_num));
            timer_.expires_from_now(boost::posix_time::millisec(50));
            timer_.async_wait(
                    boost::bind(&MulticastSender::handle_timeout, this,
                        boost::asio::placeholders::error,num_of_packet,message_num));
        }

        void sync_send(const std::string& msg,boost::asio::ip::udp::endpoint* endpoint = 0){

            if(endpoint==0)
                socket_->send_to(boost::asio::buffer(msg),endpoint_);
            else
                socket_->send_to(boost::asio::buffer(msg),*endpoint);
        }
        void sync_send(const char* msg,unsigned int  size,boost::asio::ip::udp::endpoint* endpoint = 0){
            if(endpoint == 0)
                socket_->send_to(boost::asio::buffer(msg,size),endpoint_);
            else
                socket_->send_to(boost::asio::buffer(msg,size),*endpoint);
        }



        void handle_send_to(const boost::system::error_code& error,unsigned int num_of_packet, unsigned int message_num)
        {
            std::cout << "handle send to " << std::endl;
            if(!error)
                owner->notify_sent(num_of_packet,message_num);
            else
                std::cout << "MulticastSender: " << error.message() << std::endl;

        }

        void handle_timeout(const boost::system::error_code& error,unsigned int num_of_packet, unsigned int message_num)
        {
            std::cout << "handle send to timeout " << std::endl;
            if (!error)
            {
                owner->notify_sent_timeout(num_of_packet,message_num);
            }
        }

    private:
        NetworkChannel* owner;
        boost::asio::ip::udp::endpoint endpoint_;
        boost::asio::ip::udp::socket* socket_;
        boost::asio::deadline_timer timer_;
        short multicast_port;

};



class MulticasReceiver
{
    public:
        MulticasReceiver(NetworkChannel* owner,boost::asio::io_service* io_service, boost::asio::ip::udp::socket* sock,
                const boost::asio::ip::address* listen_address,
                const boost::asio::ip::address* multicast_address,short port) : owner(owner), timer_(*io_service) 
    {
        multicast_port = port;
        boost::asio::ip::udp::endpoint listen_endpoint(
                *listen_address, multicast_port);
        if(sock == 0)
        {

            // Create the socket so that multiple may be bound to the same address.
            socket_ = new boost::asio::ip::udp::socket(*io_service);
        }
        else
        {
            socket_ = sock;
        }
        if(!socket_->is_open())
            socket_->open(listen_endpoint.protocol());
        // 			socket_->set_option(boost::asio::ip::udp::socket::reuse_address(true));
        socket_->bind(listen_endpoint);

        // Join the multicast group.
        socket_->set_option(
                boost::asio::ip::multicast::join_group(*multicast_address));
        socket_->set_option(boost::asio::ip::multicast::enable_loopback(false));


    }
        void sync_receive(char* data, unsigned int sz,boost::asio::ip::udp::endpoint* endpoint)
        {
            socket_->receive_from(boost::asio::buffer(data,sz),sender_endpoint_);
            *endpoint = sender_endpoint_;


        }
        void expire_at(){
            timer_.expires_from_now(boost::posix_time::millisec(50));
            timer_.async_wait(
                    boost::bind(&MulticasReceiver::handle_timeout, this,
                        boost::asio::placeholders::error));

        }
        void async_receive()
        {
            // // 	std::cout << "async_receive " << std::endl;
            timer_.cancel();
            socket_->async_receive_from(
                    boost::asio::buffer(data_, max_length), sender_endpoint_,
                    boost::bind(&MulticasReceiver::handle_receive_from, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

            timer_.expires_from_now(boost::posix_time::millisec(50));
            timer_.async_wait(
                    boost::bind(&MulticasReceiver::handle_timeout, this,
                        boost::asio::placeholders::error));
        }
        void handle_receive_from(const boost::system::error_code& error,
                size_t bytes_recvd)
        {
            if (!error)
            {
                //       std::cout.write(data_, bytes_recvd);
                //       std::cout << std::endl;
                owner->notify_received(data_,bytes_recvd,sender_endpoint_);
            }
            else
            {
                //	std::cout << "Receiver: " << error.message() << std::endl;
            }
        }
        void handle_timeout(const boost::system::error_code& error)
        {
            // 	cout << "handle timeout " << std::endl;
            if(!error)
                owner->notify_rec_timeout();
            else
            {
                //			std::cout << "Receiver: " << error.message() << std::endl;
            }

        }
    private:
        NetworkChannel* owner;
        boost::asio::ip::udp::socket* socket_;
        boost::asio::ip::udp::endpoint sender_endpoint_;
        boost::asio::deadline_timer timer_;
        enum { max_length = MAX_PACKET_SIZE };
        char data_[max_length];
        short multicast_port;
};


#endif // MULTICASTSENDER_RECEIVER_H
