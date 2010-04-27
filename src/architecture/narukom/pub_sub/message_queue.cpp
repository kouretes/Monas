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

#include "message_queue.h"
#include <google/protobuf/descriptor.h>

using std::map;
using std::string;

MessageQueue::MessageQueue() : Thread(false), type_string("topic") 
{
  this->publishers_buf = new map<string,MessageBuffer*>();
  this->subscribers_buf = new map<string,MessageBuffer*>();
  this->topic_tree = new TopicTree<string,MessageBuffer>();
  running = false;
  //TODO 
  //Read Configuration and create the tree!
  create_tree(topic_tree, string("topic_tree.xml"));


}

MessageQueue::MessageQueue(string configuration_file)  : Thread(false), type_string("topic")
{
  this->publishers_buf = new map<string,MessageBuffer*>();
  this->subscribers_buf = new map<string,MessageBuffer*>();
  this->topic_tree = new TopicTree<string,MessageBuffer>();
  running = false;
  //TODO 
  //Read Configuration and create the tree!
  create_tree(topic_tree, configuration_file);

}

MessageQueue::MessageQueue(const char* configuration_file)  : Thread(false), type_string("topic")
{
  this->publishers_buf = new map<string,MessageBuffer*>();
  this->subscribers_buf = new map<string,MessageBuffer*>();
  this->topic_tree = new TopicTree<string,MessageBuffer>();
  running = false;
  //TODO 
  //Read Configuration and create the tree!
  create_tree(topic_tree, string(configuration_file));

}

void MessageQueue::create_tree(TopicTree<string,MessageBuffer>* tree, string file_name)
{
  topic_tree->add_topic(string("global"));
  topic_tree->add_topic_under(string("global"),string("robot1"));
  topic_tree->add_topic_under(string("global"),string("robot2"));
  topic_tree->add_topic_under(string("robot1"),string("vision"));
  //topic_tree->add_topic_under(string("robot2"),string("vision"));
  topic_tree->add_topic_under(string("robot1"),string("motion"));
  topic_tree->add_topic_under(string("motion"),string("score"));
  

}

    void MessageQueue::remove_publisher(Publisher* pub)
    {
      pub_mutex.Lock();
      cout << "Remove Publisher" << endl;
      string pub_name = pub->getName();
      map<string,MessageBuffer*>::iterator it = publishers_buf->find(pub_name);
      if(it != publishers_buf->end() )
	publishers_buf->erase(it);
      pub_mutex.Unlock();
    }
    void MessageQueue::remove_subscriber(Subscriber* sub)
    {
      cout << "Remove Subscriber" << endl;
      sub_mutex.Lock();
      string sub_name = sub->getName();
      map<string,MessageBuffer*>::iterator it = subscribers_buf->find(sub_name);
      if(it != subscribers_buf->end() )
	subscribers_buf->erase(it);
      sub_mutex.Unlock();
    }

MessageBuffer* MessageQueue::add_publisher(Publisher* pub)//{return NULL;}//TODO};
{
  pub_mutex.Lock();
    string owner_name = pub->getName();
    
    map<string,MessageBuffer*>::iterator it = publishers_buf->find(owner_name);
      if(it != publishers_buf->end() )
	return it->second;
	
    MessageBuffer* new_msg_buf = new MessageBuffer ( owner_name );
    pub->setQueue(this);
    publishers_buf->insert ( std::make_pair<std::string,MessageBuffer*> ( owner_name,new_msg_buf ) );
    pub->setBuffer(new_msg_buf);
    pub_mutex.Unlock();
    return new_msg_buf;

}

MessageBuffer* MessageQueue::add_subscriber ( Subscriber* sub ) //{return NULL;} //TODO};
{
  sub_mutex.Lock();
    string owner_name = sub->getName();
    
    map<string,MessageBuffer*>::iterator it = subscribers_buf->find(owner_name);
      if(it != subscribers_buf->end() )
	return it->second;
    
    MessageBuffer* new_msg_buf = new MessageBuffer ( owner_name );
    sub->setQueue(this);
    subscribers_buf->insert ( std::make_pair<std::string,MessageBuffer*> ( owner_name,new_msg_buf ) );
    sub->setBuffer(new_msg_buf);
    sub_mutex.Unlock();
    return new_msg_buf;


}

bool MessageQueue::subscribe ( const char* topic, Subscriber* sub,int where )
{
  string tmp_topic = string (topic);
  string owner = sub->getName();
  std::map<std::string,MessageBuffer*>::iterator sub_it = subscribers_buf->find ( owner );
  
    if ( sub_it != subscribers_buf->end() )
    {
  
      return subscribe(tmp_topic,sub,where);
    }
    return false;
}
bool MessageQueue::subscribe (const  std::string& topic, Subscriber* sub,int where )
{
    string owner = sub->getName();
    bool result;
    sub_mutex.Lock();
    std::map<std::string,MessageBuffer*>::iterator sub_it = subscribers_buf->find ( owner );
    sub_mutex.Unlock();
    if ( sub_it != subscribers_buf->end() )
    {
      tree_mutex.Lock();
      result =  topic_tree->subscribe ( topic,sub_it->second,where );
      tree_mutex.Unlock();
      return result;
//        return true;
    }
    return false;
}


bool MessageQueue::unsubscribe ( std::string from_topic, Subscriber*  sub   )
{
    bool result;
    tree_mutex.Lock();
    cout << "!MessageQueue unsubscribe!" << endl;
    result =  topic_tree->unsubscribe( from_topic,sub->getBuffer() );
    tree_mutex.Unlock();
    return result;
}
// bool unsubscribe(const char* from_topic, Subscriber* sub )
// {
// 
//   return unsubscribe(string(from_topic),sub);
// }
//TODO
#include <boost/date_time/posix_time/posix_time.hpp>
void MessageQueue::process_queued_msg()
{
  boost::posix_time::ptime start =  boost::posix_time::microsec_clock::local_time();
  pub_mutex.Lock();
  
//   cout << "SIZEEEEEEEEEEEEEE: " <<  publishers_buf->size()  <<endl;
//   cout << "SIZEEEEEEEEEEEEEE: " <<  subscribers_buf->size()  <<endl;
//    cout << "MessageQueue " << endl;
//   bool flag = false;
  if(publishers_buf->size() > 0 )
  {
//     flag = true;
    for(map<string,MessageBuffer*>::iterator it = publishers_buf->begin(); it != publishers_buf->end(); it++)
    {
//       cout << "Message Size is " <<  endl; 
      if(it->second->size() < 1 )
      {
//         cout << "Message Size is " << it->second->size() << endl; 
				continue;
      }
      
      google::protobuf::Message* cur = it->second->remove_head();
      /*const google::protobuf::Descriptor* descriptor = cur->GetDescriptor();
      const google::protobuf::FieldDescriptor* type_descriptor = descriptor->FindFieldByName(type_string);
      const google::protobuf::FieldDescriptor* sender_field = descriptor->FindFieldByName("sender");
      string sender; 
      const google::protobuf::Reflection* reflection = cur->GetReflection();
      string tmp;
      */
			
      while(cur != 0)
      {
				/*
	tmp = reflection->GetString(*cur,type_descriptor );
	sender =  reflection->GetString(*cur,sender_field );*/
      // cout << "PRoccessing messages " <<  tmp <<  endl;
	BasicMessage* basic_msg = (BasicMessage*)cur;
	tree_mutex.Lock();
	list<MessageBuffer*>* alist =  topic_tree->message_arrived(basic_msg->topic());
	tree_mutex.Unlock();
//        cout << "DELIVERY " << alist->size() <<endl;
	for( list<MessageBuffer*>::iterator buf_it = alist->begin(); buf_it != alist->end(); buf_it++)
	{
	  if((*buf_it)->getOwner() == basic_msg->publisher() );
	  else
	  {
//  	    cout << "MQ: " << (*buf_it)->getOwner() << " " <<  basic_msg->publisher() <<endl;
	    (*buf_it)->add(cur);
	  }
	}	
	delete cur;
	cur = it->second->remove_head();
	if(cur == NULL)
	  break;
	/*descriptor = cur->GetDescriptor();
	type_descriptor = descriptor->FindFieldByName(type_string);
	sender_field = descriptor->FindFieldByName("sender");
	reflection = cur->GetReflection();*/
      }
    }
      boost::posix_time::ptime end =  boost::posix_time::microsec_clock::local_time();
  //cout << "MQ TIME :  " << boost::posix_time::to_iso_string(end-start) << endl;
  }

  //cout << "Messagqueu: " << max_pubs << " " <<  max_subs << " " << delivered << endl;
//   if(delivered == max_pubs && delivered == max_subs)
//     SetRunning(false);
  pub_mutex.Unlock();

//   sub_mutex.Unlock();
 // sleep(1);
  
}
//TODO

//void MessageQueue::finalize_queue(){}
void MessageQueue::run()
{
cout << "Starting Thread Main" << endl;
running = true;
while(running)
  process_queued_msg();

cout << "Ending Thread Main " << endl;

}