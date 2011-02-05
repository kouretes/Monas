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
#include <google/protobuf/message.h>
#include <string>
#include "stringRegistry.h"

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
/**
MessageQueue class is the message broker of Narukom.
This class is responsible for dispatching published messages to the interested subscribers.

*/
class MessageBuffer;
class MessageQueue : public Thread
{
  public:
    //friend class MessageBuffer;
    MessageQueue();
	MessageBuffer* attachPublisher(std::string const& s);
	MessageBuffer* attachSubscriber(std::string const& s);
	void subscribeTo(MessageBuffer *b, std::string const& topic , int where);
	void unsubscribeFrom(MessageBuffer *b, std::string const& topic , int where);
    void purgeBuffer( MessageBuffer *b);

    void process_queued_msg();
    virtual int Execute();
    inline void requestMailMan(MessageBuffer  * const m)
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
  	enum topicdir {ON_TOPIC, ABOVE_TOPIC, BELOW_TOPIC,ALL};
  	//String hashers
  	stringRegistry topicRegistry;
  	stringRegistry pubsubRegistry;
  	typedef struct topicdata_s{
  		std::size_t parentid;
  		std::set<std::size_t> children;

  	} topicdata;
	//Unlocked, stable and untouched while running
  	std::map<std::size_t,topicdata > topictree;
  	//Locked by pub_sub_mutex;
  	std::map<std::size_t,std::set<MessageBuffer*> > subscriptions;

    boost::mutex  pub_sub_mutex;
	//Waking up stuff
    boost::mutex  cond_mutex;
    std::set<MessageBuffer*> cond_publishers;
    std::vector<MessageBuffer*> cond_publishers_queue;
    boost::condition_variable_any cond;

    StopWatch<> agentStats;
    void create_tree();
    void addTopic(std::string const& what,std::string const& under);
    //Recursive call, UNLOCKED!
    void subscribeBelow(MessageBuffer *b, size_t topicid);
    void unsubscribeBelow(MessageBuffer *b, size_t topicid);

    };

#endif // MESSAGE_QUEUE_H
