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
#include <queue>
#include <list>
//#include "message_queue.h"

#include <iostream>
using namespace std;
using google::protobuf::Message;


template<typename T>
Buffer<T>::Buffer(std::size_t nid):
	ownerId(nid),n(NULL),c(NULL)
{

}
template<typename T>
Buffer<T>::~Buffer()
{
  boost::unique_lock<boost::mutex > data_lock(mutex);
  if(c!=NULL)
		c(this);


}



template<typename T>
void Buffer<T>::add( std::vector<T> const & tuples)
{

    boost::unique_lock<boost::mutex > data_lock(mutex);
    std::vector<msgentry>::const_iterator it= tuples.begin();
    for(;it!=tuples.end();++it)
    {
		bool filtered=false;

        for(std::list<Filter*>::iterator fit = filters.begin(); fit != filters.end(); ++fit)
		{
			if((*fit)->filter(*it) == Rejected)
			{
// 					cout << "filtered " << tuple->get_type() << endl;
				filtered=true;
				break;
			}
		}
		if(filtered) continue;
		//VERY SPECIAL POINT! WHERE POINTERS ACROSS THREADS ARE DECOUPLED

		/*google::protobuf::Message * newptr=(*it).msg->New();
        newptr->CopyFrom(*((*it).msg));
		msgentry newm= *it;
		newm.msg.reset(newptr);*/
		msgentry m=*it;
        msg_buf.push_back(m);

    }
    data_lock.unlock();
    if(tuples.size()>0&& n!=NULL)
    {
       //mq.requestMailMan(this);
		n(this);//Call notifier
    }



}

template<typename T>
bool Buffer<T>::tryadd( std::vector<T> const & tuples)
{

    if(!mutex.try_lock())
		return false;
    typename std::vector<T>::const_iterator it= tuples.begin();
    for(;it!=tuples.end();++it)
    {
    	bool filtered=false;

        for(std::list<Filter*>::iterator fit = filters.begin(); fit != filters.end(); ++fit)
		{
			if((*fit)->filter(*it) == Rejected)
			{
// 					cout << "filtered " << tuple->get_type() << endl;
				filtered=true;
				break;
			}
		}
		if(filtered) continue;

		//VERY SPECIAL POINT! WHERE POINTERS ACROSS THREADS ARE DECOUPLED

		/*google::protobuf::Message * newptr=(*it).msg->New();
        newptr->CopyFrom(*((*it).msg));
		msgentry newm= *it;
		newm.msg.reset(newptr);*/
		msgentry m=*it;
        msg_buf.push_back(m);

    }
    mutex.unlock();
    if(tuples.size()>0&& n!=NULL)
    {
       //mq.requestMailMan(this);
       n(this);//Call notifier
    }
	return true;
}

template<typename T>
void Buffer<T>::add(const T & t)
{
	boost::unique_lock<boost::mutex > data_lock(mutex);
	msgentry newm= t;
	for(std::list<Filter*>::iterator fit = filters.begin(); fit != filters.end(); ++fit)
	{
		if((*fit)->filter(t) == Rejected)
		{
// 					cout << "filtered " << tuple->get_type() << endl;
			return;
		}
	}
    msg_buf.push_back(t);
    data_lock.unlock();
    if( n!=NULL)
	{
		n(this);//Call notifier
		//mq.requestMailMan(this);
    }
}
template<typename T>
std::vector<T> Buffer<T>::remove()
{
    boost::unique_lock<boost::mutex > data_lock(mutex);
    std::vector<T> oldtupples=msg_buf;
    msg_buf.clear();
    return oldtupples;

}
template<typename T>
void Buffer<T>::add_filter(Filter* filter)
{
  boost::unique_lock<boost::mutex > data_lock(mutex);
  filters.push_back(filter);

}
template<typename T>
void Buffer<T>::remove_filter(Filter* filter)
{
  boost::unique_lock<boost::mutex > data_lock(mutex);
  for(list<Filter*>::iterator it = filters.begin(); it != filters.end(); it++)
    if(*filter == (*(*it)))
    {
      filters.erase(it);

      return;
    }
}
//Explicit template instantation for MessageBuffer :)
template class Buffer<msgentry> ;
