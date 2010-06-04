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

#include <iostream>
using namespace std;
using google::protobuf::Message;

MessageBuffer::MessageBuffer(boost::condition_variable* cv)
{
    mutex.Lock();
    owner = "";
    msg_buf = new std::vector<Tuple*>();
    mq_cv = cv;
    mutex.Unlock();
}

MessageBuffer::MessageBuffer(const std::string owner,boost::condition_variable* cv)
{
    mutex.Lock();
    this->owner = owner;
    msg_buf = new std::vector<Tuple*>();
    mq_cv =cv;
    mutex.Unlock();
}
MessageBuffer::MessageBuffer(const MessageBuffer& other)
{ 
    mutex.Lock();
    owner = other.getOwner();
    copyFrom(other);
    mq_cv = other.get_condition_variable();
    mq_cv->notify_one();
    mutex.Unlock();
}
MessageBuffer::~MessageBuffer()
{
  cout << "Destroy msg_buffer" << endl;
  mutex.Unlock();
  delete msg_buf;
  mutex.Unlock();
  
}

boost::condition_variable* MessageBuffer::get_condition_variable() const{
    return mq_cv;
}

std::vector< Tuple* >& MessageBuffer::getBuffer() const
{
  return *msg_buf;
}
string MessageBuffer::getOwner() const
{
  return owner;
}
bool MessageBuffer::isEmpty() const
{
  return msg_buf != 0 ? msg_buf->size() == 0 : true;
}

int MessageBuffer::size() const
{
  return msg_buf!=0 ? msg_buf->size() : 0;
}

void MessageBuffer::clear()
{
  msg_buf->clear();
}


bool MessageBuffer::operator==(const MessageBuffer& other) const
{
    if (this->owner == other.getOwner())
        return true;
    return false;

}

void MessageBuffer::copyFrom(const   MessageBuffer& other)
{
    mutex.Lock();
    msg_buf->clear();
    std::vector<Tuple*>& tmp = other.getBuffer();
    msg_buf->insert(msg_buf->begin(),tmp.begin(),tmp.end());
    mq_cv->notify_one();
    mutex.Unlock();
}

void MessageBuffer::mergeFrom(const MessageBuffer& other)
{
    mutex.Lock();
    std::vector<Tuple*>& tmp = other.getBuffer();
    msg_buf->insert(msg_buf->end(),tmp.begin(),tmp.end());
    mq_cv->notify_one();
    mutex.Unlock();
}

void MessageBuffer::add(Tuple* tuple)
{
    mutex.Lock();
    if(filters.size() > 0)
      for(std::list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
	if((*it)->filter(*tuple) == Rejected)
	  return;

    msg_buf->push_back(tuple);
    mq_cv->notify_one();
    mutex.Unlock();

}

Tuple* MessageBuffer::remove_head()
{
    mutex.Lock();
    Tuple* tmp = 0;
    if (msg_buf->size() > 0)
    {
        tmp = msg_buf->at(0);
        msg_buf->erase(msg_buf->begin());
    }
    mutex.Unlock();
    return tmp;
}

Tuple* MessageBuffer::remove_tail()
{
    mutex.Lock();
    Tuple* tmp = 0;
    if (msg_buf->size() > 0)
    {
        tmp = msg_buf->back();
        msg_buf->pop_back();

    }
    mutex.Unlock();
    return tmp;
}
void MessageBuffer::add_filter(Filter* filter)
{
  mutex.Lock();
  filters.push_back(filter);
  mutex.Unlock();
}
void MessageBuffer::remove_filter(const Filter& filter)
{
  mutex.Lock();
  for(list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
    if(filter == (*(*it)))
    {Filter* tmp = (*it);
      filters.erase(it);
      delete tmp;
      return;
    }
  mutex.Unlock();
}
