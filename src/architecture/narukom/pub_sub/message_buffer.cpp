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

#include "message_buffer.h"
#include "Basic.pb.h"
#include <iostream>
using namespace std;
using google::protobuf::Message;
/*
    MessageBuffer(){} //TODO
    explicit
    MessageBuffer(const std::string owner){} //TODO
    MessageBuffer(const MessageBuffer&){} //TODO
    int size(){ return msg_buf.size();}
    bool isEmpty(){ return !(msg_buf.size() > 0);}
    void clear(){ msg_buf.clear();}
    void copyFrom(const MessageBuffer&){} //TODO
    void mergeFrom(const MessageBuffer&){} //TODO
    void add(google::protobuf::Message* msg)
    {
      msg_buf.push_back(msg);
    }
    google::protobuf::Message* remove( std::vector< google::protobuf::Message* >::iterator ){return NULL;} //TODO
    google::protobuf::Message* remove_head(){return NULL;} //TODO
    google::protobuf::Message* remove_tail(){return NULL;} //TODO
    std::vector< google::protobuf::Message* >::iterator get_iterator(){return msg_buf.begin();}
    std::vector< google::protobuf::Message* >::iterator end(){ return msg_buf.end();}
    
    std::string getOwner() const {return owner;}
  private:
    std::vector<google::protobuf::Message*> msg_buf;
    std::string owner; 
    Mutex mutex;
*/
MessageBuffer::MessageBuffer()
{
	mutex.Lock();
  owner = "";
  msg_buf = new std::vector<google::protobuf::Message*>();
	mutex.Unlock();
}

MessageBuffer::MessageBuffer(const std::string owner)
{
	mutex.Lock();
this->owner = owner;
msg_buf = new std::vector<google::protobuf::Message*>();
mutex.Unlock();
}
MessageBuffer::MessageBuffer(const MessageBuffer& other)
{
	mutex.Lock();
  owner = other.getOwner();
  copyFrom(other);
	mutex.Unlock();
}

void MessageBuffer::copyFrom(const   MessageBuffer& other)
{
	mutex.Lock();
 msg_buf->clear();
 std::vector<Message*>& tmp = other.getBuffer();
 msg_buf->insert(msg_buf->begin(),tmp.begin(),tmp.end());
 mutex.Unlock();
}

void MessageBuffer::mergeFrom(const MessageBuffer& other)
{
	mutex.Lock();
 std::vector<Message*>& tmp = other.getBuffer();
 msg_buf->insert(msg_buf->end(),tmp.begin(),tmp.end());

mutex.Unlock();
}

void MessageBuffer::add(google::protobuf::Message* msg)
{
	mutex.Lock();
      google::protobuf::Message* new_msg = msg->New();
      new_msg->CopyFrom(*msg);
      msg_buf->push_back(new_msg);
			mutex.Unlock();

}

google::protobuf::Message* MessageBuffer::remove_head()
{ mutex.Lock();
  google::protobuf::Message* tmp = 0;
  if(msg_buf->size() > 0)
  {
    tmp = msg_buf->at(0);
    msg_buf->erase(msg_buf->begin());
  }
  mutex.Unlock();
  return tmp;
} //TODO
