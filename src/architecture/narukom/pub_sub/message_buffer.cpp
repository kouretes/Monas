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
#include "message_queue.h"

#include <iostream>
using namespace std;
using google::protobuf::Message;

MessageBuffer::MessageBuffer(MessageQueue *amq)
{
    boost::unique_lock<Mutex> lock(mutex);
    owner = "";
    mq=amq;

}

MessageBuffer::MessageBuffer(const std::string owner,MessageQueue *amq)
{
    boost::unique_lock<Mutex> data_lock(mutex);
    this->owner = owner;
    mq=amq;

}

MessageBuffer::~MessageBuffer()
{
boost::unique_lock<Mutex> data_lock(mutex);
  cout << "Destroy msg_buffer" << endl;


}

std::string MessageBuffer::getOwner()
{
  boost::unique_lock<Mutex> data_lock(mutex);
  return owner;
}



bool MessageBuffer::operator==( MessageBuffer& other)
{
	boost::unique_lock<Mutex> data_lock(mutex);
    if (this->owner == other.getOwner())
        return true;
    return false;

}

void MessageBuffer::add( std::vector<msgentry> & tuples)
{

    boost::unique_lock<Mutex> data_lock(mutex);
    std::vector<msgentry>::iterator it= tuples.begin();
    for(;it!=tuples.end();++it)
    {
        if(filters.size() > 0)
		{
			for(std::list<Filter*>::iterator fit = filters.begin(); fit != filters.end(); ++fit)
			{
			    if((*fit)->filter(*it) == Rejected)
				{
// 					cout << "filtered " << tuple->get_type() << endl;
					continue;
				}
			}
		}
		//VERY SPECIAL POINT! WHERE POINTERS ACROSS THREADS ARE DECOUPLED

		google::protobuf::Message * newptr=(*it).msg->New();
        newptr->CopyFrom(*((*it).msg));
		msgentry newm= *it;
		newm.msg.reset(newptr);
        msg_buf.push_back(newm);

    }
    if(tuples.size()>0)
    {
        boost::unique_lock<Mutex> cvlock(mq->cond_mutex);

        mq->cond_publishers.push_back(this);
        mq->cond.notify_one();
    }



}


void MessageBuffer::add(const msgentry & t)
{

    boost::unique_lock<Mutex> data_lock(mutex);
    msg_buf.push_back(t);




}
std::vector<msgentry> MessageBuffer::remove()
{
    boost::unique_lock<Mutex> data_lock(mutex);
    std::vector<msgentry> oldtupples=msg_buf;
    msg_buf.clear();
    return oldtupples;

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
