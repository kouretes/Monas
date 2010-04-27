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

#include "network_message.h"
#include <iostream>
#include <sstream>

using google::protobuf::uint8;
NetworkMessage::NetworkMessage(google::protobuf::uint32 msg_num)
{
	is_initialized = false;
	this->message_number = msg_num;
	this->added_packets = 0;
	timeout = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time() + boost::posix_time::millisec(100));
	type = "";
	host = "";
}
NetworkMessage::NetworkMessage(NetworkPacket* packet)
{
	
	
	type = packet->header().type();
	host = packet->header().host();
	
	timeout = boost::posix_time::to_iso_string(boost::posix_time::from_iso_string(packet->header().timestamp()) + boost::posix_time::millisec(packet->header().timeout()));
 	added_packets = 0;
	message_number = packet->header().message_num();
	
	set_packet(packet->header().packet_num()-1, packet);
	
	is_initialized = true;
	
}
void NetworkMessage::create_from_message(const google::protobuf::Message* msg)
{
	BasicMessage* basic_msg = (BasicMessage*)msg;
	unsigned int message_size = msg->ByteSize();
	uint8* buffer = new uint8[message_size];
	int num_of_packets = 1;
	msg->SerializeWithCachedSizesToArray(buffer);
	host = basic_msg->host();
	type = basic_msg->GetTypeName();
	timeout = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time() + boost::posix_time::millisec(basic_msg->timeout()));
	NetworkHeader* header = new NetworkHeader;
	header->set_host(basic_msg->host());
	header->set_message_num(this->message_number);
	header->set_timestamp(timeout);
	header->set_packet_num(1);
	header->set_type(basic_msg->GetTypeName());
	header->set_number_of_packets(num_of_packets);
	header->set_timeout(basic_msg->timeout());
	unsigned int header_size = header->ByteSize();
	NetworkPacket* tmp = new NetworkPacket;
	NetworkHeader* packet_header;
// 	std::cout <<" before cut to pieces " << (int)( MAX_PACKET_SIZE - header_size -  message_size) << std::endl;
// 	std::cout << MAX_PACKET_SIZE << " " << header_size <<  " " << message_size << std::endl;
	if((int)(MAX_PACKET_SIZE - header_size -  message_size) > 0)
	{
		packet_header = tmp->mutable_header();
		packet_header->CopyFrom(*header);
		
		tmp->set_byte_buffer(buffer,message_size);
		packets.push_back(tmp);
	}
	else
	{
		delete tmp;
		unsigned int max_header_size = header_size + 9; // 3 bytes for number_of_packets + 3 bytes for message_num + 3 bytes_packet_num
		num_of_packets = ceil(message_size / (float)(DEFAULT_PACKET_SIZE - max_header_size) );
		std::cout << "message size "<< message_size << "DEFAULT_PACKET_SIZE " << DEFAULT_PACKET_SIZE << "max_header_size " << max_header_size << std::endl;
		
		NetworkHeader* packet_header;
		unsigned int packet_size = DEFAULT_PACKET_SIZE - max_header_size;
		header->set_number_of_packets(num_of_packets);
		for(int i = 0; i < num_of_packets; i++)
		{
			tmp = new NetworkPacket;
			packet_header = tmp->mutable_header();
			
			packet_header->CopyFrom(*header);
			packet_header->set_packet_num(i+1);
			if(i < num_of_packets)
				tmp->set_byte_buffer(buffer + i*packet_size,packet_size);
			else
				tmp->set_byte_buffer(buffer + i*packet_size,message_size- i*packet_size);
			packets.push_back(tmp);
		}
		
		
		
	}
	this->added_packets = num_of_packets;
	this->is_initialized = true;
}
NetworkMessage::NetworkMessage(const NetworkHeader& network_header)
		{
		
		
		}
NetworkMessage::NetworkMessage(google::protobuf::uint32 message_number,const google::protobuf::Message& msg)
		{
// 			std::cout << "Constructor from int and msg " << std::endl;
			this->message_number = message_number;
			create_from_message(&msg);
		}
NetworkPacket* NetworkMessage::get_packet(int index) const {
			return index > packets.size() ? NULL : packets[index];
		}

void NetworkMessage::set_packet(int index, NetworkPacket* packet)
		{
			NetworkPacket* new_packet = packet->New();
			new_packet->CopyFrom(*packet);
			added_packets++;
			
			if(index < packets.size())
			{
			
				if(packets[index] == 0)
					packets[index]= new_packet;
				else
				{
					
					NetworkPacket* tmp = packets[index];
					packets[index] = new_packet;
					added_packets--;
					delete tmp;
				}
			}
			else
			{
				
				while(index - packets.size() > 0 )
					packets.push_back(0);
				packets.push_back(new_packet);
			}
		}
int NetworkMessage::get_number_of_packets() const{return added_packets;}
std::string* NetworkMessage::get_data_stream()const
{
// 	const std::vector<NetworkPacket*>::iterator it;
	std::string* result = new std::string;
	for(int i =   0; i < packets.size(); i++)
	{
		//std::cout << "packet size: " << packets[i]->byte_buffer().size() << std::endl;
		*result +=packets[i]->byte_buffer();
	}	
	return result;
}