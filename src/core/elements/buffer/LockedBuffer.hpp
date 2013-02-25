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

#ifndef BUFFER_H
#define BUFFER_H
#include "hal/mutex.h"
#include <boost/function.hpp>
#include <vector>
#include <list>


template<typename T>
class LockedBuffer
{
public:
	typedef boost::function1<void, LockedBuffer<T> *>  NotifierFncPtr;

	explicit
	LockedBuffer(std::size_t nid);
	~LockedBuffer();
	void add( std::vector<T> const & tuples);
	bool tryadd( std::vector<T> const & tuples);

	void add(const T & t);
	std::vector<T> remove();
	std::vector<T> tryremove();
	//bool operator==( MessageBuffer& other) ;
	std::size_t getOwnerID() const
	{
		return ownerId;
	};
	//MessageQueue & getQueue() {return  mq;};
	void setNotifier(NotifierFncPtr an)
	{
		KSystem::Mutex::scoped_lock data_lock(mutex);
		n = an;
	};
	void setCleanUp(NotifierFncPtr ac)
	{
		KSystem::Mutex::scoped_lock ata_lock(mutex);
		c = ac;
	};
private:

	std::vector<T> msg_buf;
	std::size_t ownerId;
	KSystem::Mutex  mutex;
	NotifierFncPtr n;
	NotifierFncPtr c;
};

using namespace KSystem;


template<typename T>
LockedBuffer<T>::LockedBuffer(std::size_t nid):
	ownerId(nid), n(NULL), c(NULL)
{
}
template<typename T>
LockedBuffer<T>::~LockedBuffer()
{
	Mutex::scoped_lock data_lock(mutex);

	if(c != NULL)
		c(this);
}



template<typename T>
void LockedBuffer<T>::add( std::vector<T> const & tuples)
{
	Mutex::scoped_lock  data_lock(mutex);
	msg_buf.reserve(msg_buf.size() + tuples.size());
	msg_buf.insert(msg_buf.end(), tuples.begin(), tuples.end());
	data_lock.unlock();

	if(tuples.size() > 0 && n != NULL)
	{
		//mq.requestMailMan(this);
		n(this);//Call notifier
	}
}

template<typename T>
bool LockedBuffer<T>::tryadd( std::vector<T> const & tuples)
{
	if(!mutex.try_lock())
		return false;

	//VERY SPECIAL POINT! WHERE POINTERS ACROSS THREADS ARE DECOUPLED
	/*google::protobuf::Message * newptr=(*it).msg->New();
	newptr->CopyFrom(*((*it).msg));
	msgentry newm= *it;
	newm.msg.reset(newptr);*/
	msg_buf.reserve(msg_buf.size() + tuples.size());
	msg_buf.insert(msg_buf.end(), tuples.begin(), tuples.end());
	mutex.unlock();

	if(tuples.size() > 0 && n != NULL)
	{
		//mq.requestMailMan(this);
		n(this);//Call notifier
	}

	return true;
}

template<typename T>
void LockedBuffer<T>::add(const T & t)
{
	Mutex::scoped_lock data_lock(mutex);
	msg_buf.push_back(t);
	data_lock.unlock();

	if( n != NULL)
	{
		n(this);//Call notifier
		//mq.requestMailMan(this);
	}
}
template<typename T>
std::vector<T> LockedBuffer<T>::remove()
{
	Mutex::scoped_lock data_lock(mutex);
	std::vector<T> oldtupples = msg_buf;
	msg_buf.clear();
	return oldtupples;
}

#endif // BUFFER_H
