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

#include "subscriber.h"
#include <iostream>
using std::string;
using std::vector;
using std::cout;
using std::endl;
using google::protobuf::Message;

Subscriber::Subscriber()
{
  subscriber_name = string("Default Subscriber");
  sub_msg_buf = 0;
}

Subscriber::Subscriber(const string& sb_name)
{
 subscriber_name = sb_name;
 cout << "Initializesubscriber" << endl;
 sub_msg_buf = 0;
}

Subscriber::Subscriber(const char* sb_name)
{
  subscriber_name = string(sb_name);
  sub_msg_buf = 0;
  cout << "Initialize subscriber" << endl;
}
Subscriber::Subscriber(const Subscriber& other)
{
  subscriber_name = other.getName();
  sub_msg_buf = new MessageBuffer();
  sub_msg_buf->copyFrom(*other.getBuffer());
}
 

Subscriber::~Subscriber()
{
  cout << "Deleting subscriber " << endl;
  if(sub_msg_buf != 0)
    sub_msg_queue->remove_subscriber(this);
  if(sub_msg_buf != 0)
    delete sub_msg_buf;
  
}
MessageBuffer* Subscriber::getBuffer() const
{
  return sub_msg_buf;
}
string Subscriber::getName() const
{
  return subscriber_name;
}

MessageQueue* Subscriber::getQueue() const
{
  return sub_msg_queue;
}

bool Subscriber::operator==(const Subscriber& sub_1)
{
  return subscriber_name == sub_1.getName();
}

void Subscriber::setBuffer(MessageBuffer* buf)
{
  sub_msg_buf = buf;
}

void Subscriber::setQueue(MessageQueue* val)
{
  sub_msg_queue = val;
}

void Subscriber::process_messages(){
   // cout << "Subscriber Process_msg  called " << endl;
    if(sub_msg_buf == 0)
    {
      if( sub_msg_queue == 0)
      {
	cout << "Subscriber with no queue" << endl;
	return;
      }
      sub_msg_queue->add_subscriber(this);
    }  
      
    while(sub_msg_buf->size() > 0)
    {
      Tuple *t = (sub_msg_buf->remove_head());
      cout  << *t << endl; 
      delete t;
    }
}