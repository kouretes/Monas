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

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H
#include <string>
#include "message_buffer.h"
#include "message_queue.h"

class MessageQueue;

class Subscriber
{
  public:
    Subscriber();
    Subscriber(const 	std::string& );
    Subscriber(const char*);
    Subscriber(const Subscriber& other);
    virtual void process_messages();
    virtual ~Subscriber();
        
    MessageBuffer* getBuffer() const  { return sub_msg_buf ; }
    std::string getName() const { return subscriber_name;}
    bool operator==(const Subscriber& sub_1)
    {
      return this->getName() == sub_1.getName();
	
    }
    
    MessageQueue* getQueue() const {return sub_msg_queue;}
    void setBuffer(MessageBuffer* buf){ sub_msg_buf = buf;}
    void setQueue(MessageQueue* val){sub_msg_queue = val;}
    

  private:
    std::string subscriber_name;
    MessageBuffer* sub_msg_buf;
    MessageQueue* sub_msg_queue;
    
};

#endif // SUBSCRIBER_H
