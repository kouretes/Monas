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
#include "message_buffer.h"
#include <string>
#include <map>
#include <utility>
#include "subscriber.h"
#include "publisher.h"

#include "topic_tree.h"
#include "../system/Mutex.h"
#include "../system/thread.h"
#ifndef TIXML_USE_STL
 #define TIXML_USE_STL
#endif
class Subscriber;
class Publisher;
/**
MessageQueue class is the message broker of Narukom.
This class is responsible for dispatching published messages to the interested subscribers.

*/
class MessageQueue : public Thread
{
  public:
    
    MessageQueue();
    MessageQueue(std::string conf);
    MessageQueue(const char*);
    
    void remove_publisher(Publisher*);
    void remove_subscriber( Subscriber*);
    MessageBuffer* add_publisher(Publisher*);
    MessageBuffer* add_publisher(Publisher*,MessageBuffer*);
    MessageBuffer* add_subscriber(Subscriber*);
    MessageBuffer* add_subscriber(Subscriber*,MessageBuffer*);
    bool subscribe(const std::string& , Subscriber* ,int );
 
    bool subscribe(const char* topic, Subscriber* owner,int where);
    bool unsubscribe(const std::string& from_topic, Subscriber*  );
    bool unsubscribe(const char* from_topic, Subscriber*  );
    bool add_topic(const std::string& new_topic);
    bool add_topic(const char* new_topic);
    bool add_topic_under(const std::string& parent,const std::string& new_topic);
    bool add_topic_under(const char* parent,const char* new_topic);
    bool remove_topic(const std::string& old_topic);
    bool remove_topic(const char* old_topic);
    void process_queued_msg();
    virtual int Execute();

  private:
    TopicTree<std::string,MessageBuffer>* topic_tree;
    std::map<std::string,MessageBuffer*>* publishers_buf;
    std::map<std::string,MessageBuffer*>* subscribers_buf;
    Mutex tree_mutex;
    Mutex pub_mutex;
    Mutex sub_mutex;
    const std::string type_string;
    boost::condition_variable cond;
    boost::mutex cond_lock;
    void create_tree(TopicTree<std::string,MessageBuffer>* tree, const std::string& file_name);
};

#endif // MESSAGE_QUEUE_H
