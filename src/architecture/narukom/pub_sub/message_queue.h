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
#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H
#include <string>
#include "stringRegistry.h"
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <map>
#include <vector>
#include <set>
#include "hal/thread.h"
#include "hal/syscall.h"

#include "tools/statMovingAverage.h"
#include "tools/stopWatch.h"

#ifndef TIXML_USE_STL
 #define TIXML_USE_STL
#endif

struct msgentry_;
typedef struct msgentry_ msgentry;
template<typename T>class Buffer;

typedef  Buffer<msgentry> MessageBuffer;
class EndPoint;

/**
MessageQueue class is the message broker of Narukom.
This class is responsible for dispatching published messages to the interested subscribers.

*/
class MessageQueue : public Thread
{
  public:
    //friend class MessageBuffer;
    MessageQueue();
    ~MessageQueue();
	MessageBuffer* makeWriteBuffer(std::string const& s);
	MessageBuffer* makeReadBuffer(std::string const& s);

    void purgeBuffer( MessageBuffer *b);

    void process_queued_msg();
    virtual int Execute();
    inline void requestMailMan( MessageBuffer  * m)
    {
		boost::unique_lock<boost::mutex > cvlock(cond_mutex);
		if(cond_publishers.find(m)==cond_publishers.end())
		{
			cond_publishers.insert(m);
			cond_publishers_queue.push_back(m);
			cond.notify_one();
		}


    };
  private:
  	//String hasher
  	EndPoint * multicast;

  	stringRegistry pubsubRegistry;


	boost::mutex  pub_sub_mutex;
  	//Locked by pub_sub_mutex;
  	std::vector< std::set<MessageBuffer*> > subscriptions;//Maps topicids to subscriber buffers
  	std::vector<std::set<MessageBuffer*> > publisherbuffers,subscriberBuffers;//Maps pubsubregistry ids to Buffers



	//Waking up stuff
    boost::mutex  cond_mutex;
    std::set<MessageBuffer*> cond_publishers;
    std::vector<MessageBuffer*> cond_publishers_queue;
    boost::condition_variable_any cond;

    StopWatch<> agentStats;

    void subscribeTo(std::size_t subid, std::size_t  topic , int where);
	void unsubscribeFrom(std::size_t subid, std::size_t  topic , int where);

    //Recursive call, UNLOCKED
    };

#endif // MESSAGE_QUEUE_H
