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

#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H
#include <google/protobuf/message.h>

#include "network_packet.pb.h"
#include <google/protobuf/stubs/common.h>
#include <cmath>
#include "network_common.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Basic.pb.h>
class NetworkMessage {
// 	friend class NetworkMessageBuffer;
	public:
		
		NetworkMessage(google::protobuf::uint32 message_number);
		NetworkMessage(NetworkPacket* packet);
		NetworkMessage(const NetworkHeader& network_header) ;
		NetworkMessage(google::protobuf::uint32 message_number,const google::protobuf::Message& boo);
		NetworkPacket* get_packet(int index) const ;
		void set_packet(int index, NetworkPacket* new_packet);
		int get_number_of_packets() const;
		void create_from_message(const google::protobuf::Message* msg);
		bool isInitilized() const {return is_initialized;}
		std::string* get_data_stream()const;
		unsigned int get_message_number() const {return message_number;}
		std::string get_timeout()  const { return timeout;}
		std::string get_type() const {return type;}
		std::string get_host()const { return host; }
		
		
	private:
		bool is_initialized;
		std::string timeout;
		std::string type;
		std::string host;
		std::vector<NetworkPacket*> packets;
		unsigned int added_packets;
		unsigned int message_number;
		
		
};

#endif // NETWORK_MESSAGE_H
