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
#ifndef MESSAGEHUB_HPP
#define MESSAGEHUB_HPP
#include <string>
#include "core/elements/StringRegistry.hpp"
#include <boost/bind.hpp>

#include <map>
#include <vector>
#include <set>

#include "hal/SystemMutex.hpp"
#include "hal/SystemCondVar.hpp"
#include "hal/SystemThread.hpp"
#include "hal/syscall.h"


#include "core/elements/math/stat/MovingAverage.hpp"
#include "core/architecture/time/StopWatch.hpp"

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif

struct msgentry_;
typedef struct msgentry_ msgentry;
template<typename T>class LockedBuffer;

typedef LockedBuffer<msgentry> MessageBuffer;
class EndPoint;

/**
MessageHub class is the message broker of Narukom.
This class is responsible for dispatching published messages to the interested subscribers.

*/
class MessageHub : public KSystem::SystemThread
{

public:
	//friend class MessageBuffer;
	MessageHub();
	~MessageHub();
	MessageBuffer* makeWriteBuffer(std::string const& s);
	MessageBuffer* makeReadBuffer(std::string const& s);

	void purgeBuffer( MessageBuffer *b);

	void process_queued_msg();
	virtual int Execute();
	inline void requestMailMan( MessageBuffer  * m)
	{
		KSystem::SystemMutex::scoped_lock cvlock(cond_mutex);

		if(m!=NULL && cond_publishers.find(m) == cond_publishers.end())
		{
			cond_publishers.insert(m);
			cond_publishers_queue.push_back(m);
			cond.notify_one();
		}
	};
private:
	//String hasher
	EndPoint * multicast;

	StringRegistry pubsubRegistry;


	KSystem::SystemMutex pub_sub_mutex;
	//Locked by pub_sub_mutex;
	std::vector< std::set<MessageBuffer*> > subscriptions;//Maps topicids to subscriber buffers
	std::vector<std::set<MessageBuffer*> > publisherbuffers, subscriberBuffers; //Maps pubsubregistry ids to Buffers



	//Waking up stuff
	KSystem::SystemMutex  cond_mutex;
	std::set<MessageBuffer*> cond_publishers;
	std::vector<MessageBuffer*> cond_publishers_queue;
	KSystem::SystemCondVar cond;

	KSystem::Time::StopWatch<> hubstats;

	void subscribeTo(std::size_t subid, std::size_t  topic , int where);
	void unsubscribeFrom(std::size_t subid, std::size_t  topic , int where);
};

#endif // MESSAGE_HUB_H
