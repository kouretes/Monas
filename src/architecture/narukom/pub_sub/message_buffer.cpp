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
    boost::unique_lock<boost::mutex > lock(mutex);
    owner = "";
    mq=amq;

}

MessageBuffer::MessageBuffer(const std::string owner,MessageQueue *amq)
{
    boost::unique_lock<boost::mutex > data_lock(mutex);
    this->owner = owner;
    mq=amq;

}

MessageBuffer::~MessageBuffer()
{
boost::unique_lock<boost::mutex > data_lock(mutex);
  cout << "Destroy msg_buffer" << endl;


}




bool MessageBuffer::operator==( MessageBuffer& other)
{
	boost::unique_lock<boost::mutex > data_lock(mutex);
    if (this->owner == other.getOwner())
        return true;
    return false;

}

void MessageBuffer::add( std::vector<msgentry> const & tuples)
{

    boost::unique_lock<boost::mutex > data_lock(mutex);
    std::vector<msgentry>::const_iterator it= tuples.begin();
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

		/*google::protobuf::Message * newptr=(*it).msg->New();
        newptr->CopyFrom(*((*it).msg));
		msgentry newm= *it;
		newm.msg.reset(newptr);*/
		msgentry m=*it;
        msg_buf.push_back(m);

    }
    data_lock.unlock();
    if(tuples.size()>0&& mq!=NULL)
    {
       mq->requestMailMan(this);
    }



}


bool MessageBuffer::tryadd( std::vector<msgentry> const & tuples)
{

    if(!mutex.try_lock())
		return false;
    std::vector<msgentry>::const_iterator it= tuples.begin();
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

		/*google::protobuf::Message * newptr=(*it).msg->New();
        newptr->CopyFrom(*((*it).msg));
		msgentry newm= *it;
		newm.msg.reset(newptr);*/
		msgentry m=*it;
        msg_buf.push_back(m);

    }
    mutex.unlock();
    if(tuples.size()>0&& mq!=NULL)
    {
       mq->requestMailMan(this);
    }
	return true;
}


void MessageBuffer::add(const msgentry & t)
{
	boost::unique_lock<boost::mutex > data_lock(mutex);
	msgentry newm= t;
    msg_buf.push_back(t);
    data_lock.unlock();
    if( mq!=NULL)
	{
		mq->requestMailMan(this);
    }
}
std::vector<msgentry> MessageBuffer::remove()
{
    boost::unique_lock<boost::mutex > data_lock(mutex);
    std::vector<msgentry> oldtupples=msg_buf;
    msg_buf.clear();
    return oldtupples;

}

void MessageBuffer::add_filter(Filter* filter)
{
  boost::unique_lock<boost::mutex > data_lock(mutex);
  filters.push_back(filter);

}
void MessageBuffer::remove_filter(Filter* filter)
{
  boost::unique_lock<boost::mutex > data_lock(mutex);
  for(list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
    if(*filter == (*(*it)))
    {
      filters.erase(it);

      return;
    }
}
