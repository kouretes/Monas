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

#include "network_message_buffer.h"

std::vector<unsigned>* NetworkMessageBuffer::add_packet(NetworkPacket* packet)
{
    network_message_set::iterator it = network_messages.find(boost::make_tuple(packet->header().host(),packet->header().message_num()));

    NetworkMessage* msg;
    std::vector< unsigned int >* result= new std::vector<unsigned>;
    if(it == network_messages.end())
    {

        msg= new NetworkMessage(packet);
        if(packet->header().number_of_packets() == 1 && packet->header().packet_num() == 1)
        {
            owner->deliver(msg,0);
            return result;
        }
        else
            network_messages.insert(msg);
    }
    else
    {

        (*it)->set_packet(packet->header().packet_num()-1,packet);
        msg = (*it);

        if(packet->header().number_of_packets() ==  (*it)->get_number_of_packets())
            owner->deliver(*it,this);
        // 				{

        // 				}
    }
    cout << "Maybe Missing" << endl;
    for(int i = 0; i <= packet->header().packet_num()-1; i++)
    {
        if(msg->get_packet(i) == 0)
        {
            cout << i << " " ;
            result->push_back(i);
        }
    }
    cout << endl;
    return result;
}


void NetworkMessageBuffer::add_message( NetworkMessage* msg)
{
    number_of_messages++;
    const std::string& msg_host = msg->get_host();

    network_message_set::iterator it = network_messages.find(boost::make_tuple(msg->get_host(),msg->get_message_number()));
    //std::cout << "host: " << (*it)->get_message_number() << std::endl;
    // 	    msg = *it;
    if(it == network_messages.end())
        network_messages.insert(msg);
    else
    {
        std::cout << "Error Already message from that host Replacing" << std::endl;
        network_messages.erase(it);
        network_messages.insert(msg);
        number_of_messages--;
    }

    // 		  std::cout << "SIZE " << network_messages.size() << std::endl;
}

NetworkMessage* NetworkMessageBuffer::get_message(std::string host, int message_number)
{
    network_message_set::iterator it = network_messages.find(boost::make_tuple(host,message_number));
    if(it != network_messages.end())
        return (*it);
}




NetworkMessage* NetworkMessageBuffer::remove( std::string host, int message_number ){
    network_message_set::iterator it = network_messages.find(boost::make_tuple(host,message_number));
    NetworkMessage* result = 0;
    if(it != network_messages.end())
    {
        result = *it;
        network_messages.erase(it);
    }
    return result;


}




NetworkMessage* NetworkMessageBuffer:: remove_head(){
    NetworkMessage* result = *network_messages.begin();
    network_messages.erase(network_messages.begin());
}

