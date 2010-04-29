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

#ifndef NETWORK_MESSAGE_BUFFER_H
#define NETWORK_MESSAGE_BUFFER_H
#include "network_message.h"
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "network_channel.h"
#include "../system/Mutex.h"
// class NetworkChannel;

typedef boost::multi_index::multi_index_container<
NetworkMessage*,
 boost::multi_index::indexed_by<
								boost::multi_index::ordered_unique<			
								boost::multi_index::composite_key<
											NetworkMessage,
											boost::multi_index::const_mem_fun<NetworkMessage,std::string,&NetworkMessage::get_host>,
											boost::multi_index::const_mem_fun<NetworkMessage,unsigned int ,&NetworkMessage::get_message_number> > >,											
					boost::multi_index::ordered_non_unique< boost::multi_index::const_mem_fun<NetworkMessage,std::string,&NetworkMessage::get_timeout> >
 >
> network_message_set;
class NetworkMessageBuffer {
	
	

	public:
		
    explicit
    NetworkMessageBuffer( NetworkChannel* const owner) : owner(owner){
			number_of_messages = 0;
		}
    NetworkMessageBuffer(const NetworkMessageBuffer& other): owner(other.getOwner()){number_of_messages = 0;}
    int size(){ return number_of_messages;}
    bool isEmpty(){ return (number_of_messages == 0);}
    void clear(){ clear();}
    void copyFrom(const NetworkMessageBuffer&);
    void mergeFrom(const NetworkMessageBuffer&);
		
		
    std::vector<unsigned>* add_packet(NetworkPacket* packet);

    void add_message( NetworkMessage* msg);


    
    NetworkMessage* get_message(std::string host, int message_number);





    NetworkMessage* remove( std::string host, int message_number );



    NetworkMessage* remove_head();


    // 		NetworkMessage* remove( std::string host, std::string type ){}
    const network_message_set& get_message_set() const {return network_messages;}
    //     std::vector< google::protobuf::Message* >::iterator get_iterator();
    //     std::vector< google::protobuf::Message* >::iterator end();
    //     std::vector<google::protobuf::Message*>&   getBuffer() const   {return *n;}
    NetworkChannel* getOwner() const {return owner;}



    private:
    void cleanup();
    std::string current_timestamp;
    NetworkChannel* const owner;
    network_message_set network_messages;
    unsigned int number_of_messages;
    // 		Mutex mx;

};

#endif // NETWORK_MESSAGE_BUFFER_H
