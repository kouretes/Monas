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

#include "udp_multicast_channel.h"
#include <google/protobuf/descriptor.h>
#include "Nack.pb.h"

void UdpMulticastChannel::run()
{
// 	std::cout << "Run PBUS" << std::endl;
    io_service->reset();
    receive_from_network();
    if (Subscriber::getBuffer() != 0)
        if (Subscriber::getBuffer()->size() > 0)
            process_messages();
//  	std::cout << "Run PBUS exit" << std::endl;
    write_to_network();
//  	std::cout << "Run PBUS efsdfxit" << std::endl;
    io_service->run();

// //   	std::cout << "Run sender" << std::endl;

    sched_yield();

}

void UdpMulticastChannel::process_messages()
{
    MessageBuffer* sub_buf = Subscriber::getBuffer();
    google::protobuf::Message* cur = sub_buf->remove_head();
    NetworkMessage* new_network_message;
    const google::protobuf::Descriptor* descriptor;
    const google::protobuf::Reflection* reflection;
    const google::protobuf::FieldDescriptor* string_field;
    while (cur != 0)
    {
        descriptor = cur->GetDescriptor();
        reflection = cur->GetReflection();
        string_field =  descriptor->FindFieldByName("host");
        reflection->SetString(cur,string_field,NetworkChannel::get_host_name());
        new_network_message = new NetworkMessage(NetworkChannel::get_next_message_number(),*cur);
        outgoing->add_message(new_network_message);
				cout << outgoing->size();
        delete cur;
        cur = sub_buf->remove_head();
    }
}
void UdpMulticastChannel::publish(google::protobuf::Message* msg)
{
    //cout << "Publish " << msg->GetTypeName() << std::endl;
    Publisher::publish(msg);

}
void UdpMulticastChannel::deliver(NetworkMessage* msg,NetworkMessageBuffer * buf) {
		if(msg->get_packet(0)->header().type() != "NackMessage")
			NetworkChannel::deliver(msg,buf);
		else
		{
			NackMessage nack_msg;
			std::string* tmp_str = msg->get_data_stream();
			nack_msg.ParsePartialFromString(*tmp_str);
			respond_nack(nack_msg);
			delete tmp_str;
		}

}
void UdpMulticastChannel::notify_sent(unsigned int packet_num, unsigned int message_num) {
    std::cout << "packet: " << packet_num << " of " << message_num << " sendt " << std::endl;
    if (message_num > last_send[0])
    {
        last_send[0] = message_num;
        last_send[1] = -1;
    }
    else if (message_num == last_send[0] )
        last_send[1] = packet_num;
}
void UdpMulticastChannel::notify_sent_timeout(unsigned int packet_num, unsigned int message_num) {
    std::cout << " sent timeout packet:  " << packet_num << " of " << message_num << std::endl;
    timed_out.push_back(std::make_pair<unsigned, unsigned>(message_num,packet_num));
    io_service->stop();
}
void UdpMulticastChannel::notify_rec_timeout()
{
//  			std::cout << "receive timeout adfda " << std::endl;
// 				io_service->stop();
// 				io_service->
    io_service->stop();


// 				receive_from_network();
}
void UdpMulticastChannel::notify_received(char* data, size_t sz,boost::asio::ip::udp::endpoint sender_endpoint)
{
    std::vector<unsigned>* lost;
    NetworkPacket* packet = new NetworkPacket;
    if (packet->ParseFromArray(data,sz))
    {
        lost = incoming->add_packet(packet);
        if (lost->size() > 0 )
        {
            send_nack(packet,sender_endpoint,lost);
        }
    }
    //std::cout << "Received: " << sz << " bytes " << std::endl;
    receive_from_network();
}


void UdpMulticastChannel::write_to_network()
{
    const network_message_set&  network_set= outgoing->get_message_set();
    network_message_set::iterator it =  network_set.find(boost::make_tuple(NetworkChannel::get_host_name(),last_send[0]));
    int i = 0;
    if (it == network_set.end())
        it = network_set.begin();
    else
    {
        i = last_send[1];
        if (i == 0)
            it++;
    }
    while (it != network_set.end())
    {
        for (; i < (*it)->get_number_of_packets(); i++)
        {
            NetworkPacket* cur_packet = (*it)->get_packet(i);
// 						if(i!=2)
							sender->sync_send(cur_packet->SerializeAsString());
            last_send[1]++;
        }
        last_send[1] = 0;
        last_send[0]++;
        it++;
    }

}

void UdpMulticastChannel::send_nack(NetworkPacket* packet, boost::asio::ip::udp::endpoint& endpoint, std::vector< unsigned >* missing)
{
	static NackMessage nack_msg;
	static NetworkMessage* net_msg;
	nack_msg.clear_missing_packets();
	
	nack_msg.set_receiver(packet->header().host());
	bool out_of_order = false;
	if( missing->at(0) < packet->header().packet_num())
		out_of_order =true;
	if(net_msg==0 && out_of_order)
	{
		nack_msg.set_host(NetworkChannel::get_host_name());
		nack_msg.set_message_num( packet->header().message_num());
		nack_msg.set_publisher("udp_multicast_channel");
		nack_msg.set_timeout(100);
		for(int i =0; i < missing->size(); i++)
			nack_msg.add_missing_packets(missing->at(i));
		net_msg = new NetworkMessage(0,nack_msg);
	}
	if(out_of_order)
		sender->sync_send(net_msg->get_packet(0)->SerializeAsString());
}

void UdpMulticastChannel::respond_nack(const NackMessage& nack_msg )
{
	NetworkMessage* msg = outgoing->get_message(NetworkChannel::get_host_name(),nack_msg.message_num());
	for(int i = 0; i < nack_msg.missing_packets_size(); i++)
		this->sender->sync_send(msg->get_packet(nack_msg.missing_packets(i))->SerializeAsString());
	
}


void UdpMulticastChannel::read_from_network()
{
    boost::asio::ip::udp::endpoint sender_endpoint;
    unsigned int sz = MAX_PACKET_SIZE;
    std::vector<unsigned>* lost;
    char* data = new char[sz];
    receiver->sync_receive(data,sz,&sender_endpoint);
    NetworkPacket* packet = new NetworkPacket;
    if (packet->ParseFromArray(data,sz))
    {
        lost = incoming->add_packet(packet);
        if (lost->size() > 0 )
        {
            //send message to endpoint to resent data
            //sender->sync_send()
        }
    }

    delete data;
}
void UdpMulticastChannel::receive_from_network()
{
    receiver->async_receive();
}

