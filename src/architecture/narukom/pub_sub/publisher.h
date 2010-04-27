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

#ifndef PUBLISHER_H
#define PUBLISHER_H
#include <string>
#include "message_buffer.h"
#include <google/protobuf/message.h>
#include "message_queue.h"
#include "Basic.pb.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
class MessageQueue;
class Publisher
{
  public:
    Publisher();
    Publisher(std::string str);
    Publisher(const char* );
    Publisher(const Publisher&);
    virtual ~Publisher();
    virtual void publish(google::protobuf::Message* msg);
    
    bool operator==(const Publisher& pub_1)
    {
      return this->getName() == pub_1.getName();
	
    }
    
    MessageBuffer* getBuffer()const { return pub_msg_buf ; }
    std::string getName() const { return publisher_name;}
    MessageQueue* getQueue() const {return pub_msg_queue;}
   void setBuffer(MessageBuffer* buf){ pub_msg_buf = buf;} 
    void setQueue(MessageQueue* val){pub_msg_queue = val;}
   
  private:
    std::string publisher_name;
    MessageBuffer* pub_msg_buf;
    MessageQueue* pub_msg_queue; // in order to keep it sane
  
};

#endif // PUBLISHER_H
