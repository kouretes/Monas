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

#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/message.h>
#include <vector>
#include "Mutex.h"
class MessageBuffer
{
  public:
    MessageBuffer();
    explicit
    MessageBuffer(const std::string owner);
    MessageBuffer(const MessageBuffer&);
    int size(){ return msg_buf->size();}
    bool isEmpty(){ return !(msg_buf->size() > 0);}
    void clear(){ msg_buf->clear();}
    void copyFrom(const MessageBuffer&);
    void mergeFrom(const MessageBuffer&);
    void add(google::protobuf::Message* msg);
    
    google::protobuf::Message* remove( std::vector< google::protobuf::Message* >::iterator );
    google::protobuf::Message* remove_head();
    google::protobuf::Message* remove_tail();
    std::vector< google::protobuf::Message* >::iterator get_iterator();
    std::vector< google::protobuf::Message* >::iterator end();
    std::vector<google::protobuf::Message*>&   getBuffer() const   {return *msg_buf;}
    std::string getOwner() const {return owner;}
    
  private:
    std::vector<google::protobuf::Message*>* msg_buf;
    std::string owner; 
    Mutex mutex;
  
};

#endif // MESSAGE_BUFFER_H
