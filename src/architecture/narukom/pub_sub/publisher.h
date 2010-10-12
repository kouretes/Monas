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
    virtual ~Publisher();
    virtual void publish(const  msgentry & msg);
	virtual void publish(std::vector<msgentry> vec);
	//    virtual void publish(Serializable* msg ,const std::string& topic,unsigned timeout = 500, const std::string& destination = "");

    bool operator==(const Publisher& pub_1);//


    std::string getName() const;// { return publisher_name;}
    void setBuffer(MessageBuffer* buf);//{ pub_msg_buf = buf;}
    void setQueue(MessageQueue* val);//{pub_msg_queue = val;}

  private:
    std::string publisher_name;
    MessageBuffer* pub_msg_buf;
  protected:
    MessageQueue* pub_msg_queue; // in order to keep it sane

};

#endif // PUBLISHER_H
