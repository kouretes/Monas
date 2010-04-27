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
  sub_msg_buf = NULL;
}

Subscriber::Subscriber(const string& sb_name)
{
 this->subscriber_name = sb_name;
 cout << "Initializesubscriber" << endl;
 sub_msg_buf = NULL;
}

Subscriber::Subscriber(const char* sb_name)
{
  this->subscriber_name = string(sb_name);
  sub_msg_buf = NULL;
  cout << "Initialize subscriber" << endl;
}
Subscriber::Subscriber(const Subscriber& other)
{
  this->subscriber_name = other.getName();
  sub_msg_buf = new MessageBuffer();
  sub_msg_buf->copyFrom(*other.getBuffer());
}
 

Subscriber::~Subscriber()
{
  if(sub_msg_buf != NULL)
    sub_msg_queue->remove_subscriber(this);
  if(sub_msg_buf != NULL)
    delete sub_msg_buf;
  
}

void Subscriber::process_messages(){
   // cout << "Subscriber Process_msg  called " << endl;
    if(sub_msg_buf == NULL)
    {
      if( sub_msg_queue == NULL)
      {
	cout << "Subscriber with no queue" << endl;
	return;
      }
      sub_msg_queue->add_subscriber(this);
    }  
      
    while(sub_msg_buf->size() > 0)
    {
      cout  << sub_msg_buf->remove_head()->DebugString() << endl; 
    }
}