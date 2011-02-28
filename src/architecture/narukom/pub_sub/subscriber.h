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
#include <vector>
#include "msg.h"

class MessageQueue;
//Forward Decleration of MessageBuffer
template<typename T>class Buffer;
typedef  Buffer<msgentry> MessageBuffer;

class Subscriber
{
  public:
    Subscriber();
    Subscriber(const 	std::string& );

    virtual void process_messages();
    std::vector<msgentry> remove();

    virtual ~Subscriber();

    std::string const getSubscriberName() const { return subscriber_name;}
    MessageBuffer *getSubscriberBuffer() const { return sub_msg_buf;}
    void attachSubscriberToMessageQueue(MessageQueue & q);
    void subscribeTo(std::string const& topic , int where);
    void unsubscribeFrom(std::string const& topic , int where);


  private:
    std::string subscriber_name;
    MessageBuffer* sub_msg_buf;
    MessageQueue *mq;
};

#endif // SUBSCRIBER_H
