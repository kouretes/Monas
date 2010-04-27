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

#include "network_channel.h"
#include "network_message_buffer.h"
#include "Basic.pb.h"
#include "pingpong.pb.h"
//#include "image.pb.h"
#include "network_packet.pb.h"
using std::cout;
using std::endl;
void NetworkChannel::deliver(NetworkMessage* msg,NetworkMessageBuffer* buf){
	google::protobuf::Message* result;
	if(msg->get_type() == "BasicMessage")
	{
		result = new BasicMessage;
	}
	else if(msg->get_type() == "PingMessage")
	{
		result = new PingMessage;
	}
	else if(msg->get_type() == "PongMessage")
	{
		result = new PongMessage;
	}
//	else if(msg->get_type() == "Image")
//	{
//		result = new Image;
//	}
	else
	{
	
		result = 0;
		cout << "Unknown Message Type " << endl;
	}
	if(result != 0)
		result->ParsePartialFromString(*msg->get_data_stream());
	if(buf != 0)
		buf->remove(msg->get_host(),msg->get_message_number());
	publish(result);
}
