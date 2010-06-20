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

MessageBuffer::MessageBuffer(Mutex* queue_mx ,boost::condition_variable_any* cv)
{
    boost::unique_lock<Mutex> lock(mutex);
    owner = "";
    msg_buf = new std::list<Tuple*>();
    mq_cv = cv;
		mq_mutex = queue_mx;
}

Mutex* MessageBuffer::get_queue_mutex()
{
	return mq_mutex;
}
MessageBuffer::MessageBuffer(const std::string owner,Mutex* queue_mx ,boost::condition_variable_any* cv)
{
    boost::unique_lock<Mutex> data_lock(mutex);
    this->owner = owner;
    msg_buf = new std::list<Tuple*>();
    mq_cv =cv;
    mq_mutex = queue_mx;

}

MessageBuffer::~MessageBuffer()
{
boost::unique_lock<Mutex> data_lock(mutex);
  cout << "Destroy msg_buffer" << endl;
	if(msg_buf != 0)
	{
		msg_buf->clear();
	  delete msg_buf;
	}
  
}

boost::condition_variable_any* MessageBuffer::get_condition_variable() {
boost::unique_lock<Mutex> data_lock(mutex);
	return mq_cv;
}

std::list< Tuple* >& MessageBuffer::getBuffer()
{
boost::unique_lock<Mutex> data_lock(mutex);
  return *msg_buf;
}
std::string MessageBuffer::getOwner() 
{
boost::unique_lock<Mutex> data_lock(mutex);
  return owner;
}
bool MessageBuffer::isEmpty()
{
boost::unique_lock<Mutex> data_lock(mutex);
  return msg_buf != 0 ? msg_buf->size() == 0 : true;
}

int MessageBuffer::size()
{
    boost::unique_lock<Mutex>  mx_lock(mutex);
  int result = msg_buf!=0 ? msg_buf->size() : 0;
	return result;
}

void MessageBuffer::clear()
{
	boost::unique_lock<Mutex> data_lock(mutex);
  msg_buf->clear();
}


bool MessageBuffer::operator==( MessageBuffer& other)
{
	boost::unique_lock<Mutex> data_lock(mutex);
    if (this->owner == other.getOwner())
        return true;
    return false;

}

void MessageBuffer::copyFrom(   MessageBuffer& other)
{
    boost::unique_lock<Mutex> data_lock(mutex);
		boost::lock_guard<Mutex>  mx_lock(*mq_mutex);
		if(msg_buf != 0)
			msg_buf->clear();
		else
			msg_buf =  new std::list<Tuple*>();
    std::list<Tuple*>& tmp = other.getBuffer();
    msg_buf->insert(msg_buf->begin(),tmp.begin(),tmp.end());
// 		mq_mutex->lock();
    mq_cv->notify_one();
// 		mq_mutex->unlock();
    
    
}

void MessageBuffer::mergeFrom( MessageBuffer& other)
{
		
    boost::unique_lock<Mutex> data_lock(mutex);
	
    std::list<Tuple*>& tmp = other.getBuffer();
    msg_buf->insert(msg_buf->end(),tmp.begin(),tmp.end());
    data_lock.unlock();
    mq_mutex->lock();
    mq_cv->notify_one();
		mq_mutex->unlock();
    
    
}

void MessageBuffer::add(Tuple* tuple)
{

    boost::unique_lock<Mutex> data_lock(mutex);
    boost::lock_guard<Mutex>  mx_lock(*mq_mutex);
    if(filters.size() > 0)
		{
			for(std::list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
			{
				if((*it)->filter(*tuple) == Rejected)
				{
// 					cout << "filtered " << tuple->get_type() << endl;
					return;
				}
			}
		}
		Tuple* tmp = new Tuple(*tuple);
    msg_buf->push_back(tmp);
    
    mq_cv->notify_one();
		
    

}

Tuple* MessageBuffer::remove_head()
{
    if(!mutex.try_lock())
			return 0;
    Tuple* tmp = 0;
    if (msg_buf->size() > 0)
    {
        tmp = msg_buf->front();
        msg_buf->erase(msg_buf->begin());
    }
    mutex.unlock();
    return tmp;
}

Tuple* MessageBuffer::remove_tail()
{
    boost::unique_lock<Mutex> data_lock(mutex);
    Tuple* tmp = 0;
    if (msg_buf->size() > 0)
    {
        tmp = msg_buf->back();
        msg_buf->pop_back();

    }
    
    return tmp;
}
void MessageBuffer::add_filter(Filter* filter)
{
  boost::unique_lock<Mutex> data_lock(mutex);
  filters.push_back(filter);
  
}
void MessageBuffer::remove_filter(Filter* filter)
{
  boost::unique_lock<Mutex> data_lock(mutex);
  for(list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
    if(*filter == (*(*it)))
    {
      filters.erase(it);
      
      return;
    }
}
