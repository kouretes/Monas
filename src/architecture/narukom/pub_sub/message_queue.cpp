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
#include "../tools/XML.h"


using std::map;
using std::string;

MessageQueue::MessageQueue() : Thread(false), type_string("topic")
{
    this->publishers_buf = new map<string,MessageBuffer*>();
    this->subscribers_buf = new map<string,MessageBuffer*>();
    this->topic_tree = new TopicTree<string,MessageBuffer>();
    running = false;
    create_tree(topic_tree, string("topic_tree.xml"));


}

MessageQueue::MessageQueue(string configuration_file)  : Thread(false), type_string("topic")
{
    this->publishers_buf = new map<string,MessageBuffer*>();
    this->subscribers_buf = new map<string,MessageBuffer*>();
    this->topic_tree = new TopicTree<string,MessageBuffer>();
    create_tree(topic_tree, configuration_file);

}

MessageQueue::MessageQueue(const char* configuration_file)  : Thread(false), type_string("topic")
{
    this->publishers_buf = new map<string,MessageBuffer*>();
    this->subscribers_buf = new map<string,MessageBuffer*>();
    this->topic_tree = new TopicTree<string,MessageBuffer>();
    create_tree(topic_tree, string(configuration_file));
}

void MessageQueue::create_tree(TopicTree<string,MessageBuffer>* tree,const  string& file_name)
{
    string parent = "";
    string topic;
    XML configFile(file_name);
    if (!configFile.IsLoadedSuccessfully())
    {
        cout << "Could not load file " << file_name << endl;
        topic_tree->add_topic(string("global"));
        topic_tree->add_topic_under(string("global"),string("motion"));
        topic_tree->add_topic_under(string("global"),string("sensors"));
        topic_tree->add_topic_under(string("global"),string("vision"));
        topic_tree->add_topic_under(string("global"),string("behavior"));
        topic_tree->add_topic_under(string("global"),string("localization"));
        topic_tree->add_topic_under(string("global"),string("communication"));
        topic_tree->add_topic_under(string("motion"),string("score"));
        return;
    }
    cout << "File: " << file_name << " Loaded " << endl;
    const string tree_str("tree");
    std::vector<XMLNode<std::string,std::string,std::string> > trees = configFile.QueryElement<std::string,std::string,std::string>(tree_str,NULL);

    for (std::vector<XMLNode<std::string,std::string,std::string> >::iterator it = trees.begin(); it != trees.end(); it++)
    {
        cout << "inside for " << endl;
        std::vector<XMLNode<std::string,std::string,std::string> > nodes;
        nodes = configFile.QueryElement(string("node"),&(*it));
        for (unsigned int j =0; j < nodes.size(); j++)
        {
            string parent = nodes[j].attrb["parent"];
            if (parent == "none" || parent == "")
            {
                cout << "topic tree " << nodes[j].value << endl;
                topic_tree->add_topic(nodes[j].value);
            }
            else
            {
                cout << "topic tree under " << parent << " " << nodes[j].value << endl;
                topic_tree->add_topic_under(parent,nodes[j].value);
            }
        }

    }

}


bool MessageQueue::add_topic(const std::string& new_topic)
{
  return topic_tree->add_topic(new_topic);
}

bool MessageQueue::add_topic(const char* new_topic)
{
  std::string tmp_topic(new_topic);
  return this->add_topic(tmp_topic);
}

bool MessageQueue::add_topic_under(const std::string& parent,const std::string& new_topic)
{
  return topic_tree->add_topic_under(parent,new_topic);
}

bool MessageQueue::add_topic_under(const char* parent,const char* new_topic)
{
  std::string tmp_parent(parent);
  std::string tmp_topic(new_topic);
  return topic_tree->add_topic_under(tmp_parent,tmp_topic);
}

bool MessageQueue::remove_topic(const std::string& old_topic)
{
  return topic_tree->delete_topic(old_topic);
}
bool MessageQueue::remove_topic(const char* old_topic)
{
  std::string tmp_old_topic(old_topic);
  return topic_tree->delete_topic(tmp_old_topic);
}


void MessageQueue::remove_publisher(Publisher* pub)
{
    pub_mutex.Lock();
    cout << "Remove Publisher" << endl;
    string pub_name = pub->getName();
    map<string,MessageBuffer*>::iterator it = publishers_buf->find(pub_name);
    if (it != publishers_buf->end() )
        publishers_buf->erase(it);
    pub_mutex.Unlock();
}
void MessageQueue::remove_subscriber(Subscriber* sub)
{
    cout << "Remove Subscriber" << endl;
    sub_mutex.Lock();
    string sub_name = sub->getName();
    map<string,MessageBuffer*>::iterator it = subscribers_buf->find(sub_name);
    if (it != subscribers_buf->end() )
        subscribers_buf->erase(it);
    sub_mutex.Unlock();
}

MessageBuffer* MessageQueue::add_publisher(Publisher* pub)//{return NULL;}//TODO};
{
    pub_mutex.Lock();
    string owner_name = pub->getName();

    map<string,MessageBuffer*>::iterator it = publishers_buf->find(owner_name);
    if (it != publishers_buf->end() )
    {
	pub_mutex.UnLock();
        return it->second;
    }
    MessageBuffer* new_msg_buf = new MessageBuffer ( owner_name,&cond);
    pub->setQueue(this);
    publishers_buf->insert ( std::make_pair<std::string,MessageBuffer*> ( owner_name,new_msg_buf ) );
    pub->setBuffer(new_msg_buf);
    pub_mutex.Unlock();
    return new_msg_buf;

}

MessageBuffer* MessageQueue::add_publisher(Publisher* pub , MessageBuffer* buf)
{
  if(pub != 0 && buf !=0)
  {
    pub->setQueue(this);
    pub_mutex.Lock();
    publishers_buf->insert(std::make_pair<std::string,MessageBuffer*> ( pub->getName(),buf ));
    pub_mutex.Unlock();
    return buf;
  }
  cout << " Either buffer or Publisher is null " << endl;
  return 0;

}


MessageBuffer* MessageQueue::add_subscriber ( Subscriber* sub ) //{return NULL;} //TODO};
{
    sub_mutex.Lock();
    string owner_name = sub->getName();

    map<string,MessageBuffer*>::iterator it = subscribers_buf->find(owner_name);
    if (it != subscribers_buf->end() )
    {
       sub_mutex.UnLock();
       return it->second;
    }
    MessageBuffer* new_msg_buf = new MessageBuffer ( owner_name,&cond );
    sub->setQueue(this);
    subscribers_buf->insert ( std::make_pair<std::string,MessageBuffer*> ( owner_name,new_msg_buf ) );
    sub->setBuffer(new_msg_buf);
    sub_mutex.Unlock();
    return new_msg_buf;


}
MessageBuffer* MessageQueue::add_subscriber(Subscriber* sub, MessageBuffer* buf )
{
  if(sub != 0 && buf !=0)
  {
    sub->setQueue(this);
    sub_mutex.Lock();
    subscribers_buf->insert(std::make_pair<std::string,MessageBuffer*> ( sub->getName(),buf ));
    sub_mutex.Unlock();
    return buf;
  }
  cout << " Either buffer or Subscriber is null " << endl;
  return 0;

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
        MessageBuffer* subscriber = sub_it->second;
        result =  topic_tree->subscribe ( topic,subscriber,where );
        tree_mutex.Unlock();
        return result;
        //        return true;
    }
    else
    {
        cout << "Subscriber was not added in message queue\nAutomatically added" << endl;
        this->add_subscriber(sub);
        tree_mutex.Lock();
        result =  topic_tree->subscribe ( topic,sub->getBuffer(),where );
        tree_mutex.Unlock();
        return result;


    }
    return false;
}


bool MessageQueue::unsubscribe (const std::string& from_topic, Subscriber*  sub   )
{
    bool result;
    tree_mutex.Lock();
    cout << "!MessageQueue unsubscribe!" << endl;
    result =  topic_tree->unsubscribe( from_topic,sub->getBuffer() );
    tree_mutex.Unlock();
    return result;
}
 bool MessageQueue::unsubscribe(const char* from_topic, Subscriber* sub )
 {
   std::string tmp_from_topic(from_topic);
   return unsubscribe(tmp_from_topic,sub);
 }

void MessageQueue::process_queued_msg()
{
   bool new_messages_to_deliver = false;
    pub_mutex.Lock();
    if (publishers_buf->size() > 0 )
    {
        for (map<string,MessageBuffer*>::iterator it = publishers_buf->begin(); it != publishers_buf->end(); it++)
        {
            if (it->second->size() < 1 )
            {
                continue;
            }

            Tuple* cur = it->second->remove_head();

            while (cur != 0)
            {
	      new_messages_to_deliver = true;
                tree_mutex.Lock();
                list<MessageBuffer*>* alist =  topic_tree->message_arrived(cur->get_topic());
                tree_mutex.Unlock();
                for ( list<MessageBuffer*>::iterator buf_it = alist->begin(); buf_it != alist->end(); buf_it++)
                {
                    if ((*buf_it)->getOwner() == cur->get_publisher() );
                    else
                    {
                        (*buf_it)->add(cur);
                    }
                }
                delete cur;
                cur = it->second->remove_head();
                if (cur == 0)
                    break;
            }
        }
    }
    if(!new_messages_to_deliver)
    {
      pub_mutex.Unlock();
      boost::unique_lock<boost::mutex>  lock(cond_lock);
      cond.wait(lock);

    }


    pub_mutex.Unlock();

}


//void MessageQueue::finalize_queue(){}
int MessageQueue::Execute()
{
//   cout << "Starting Thread Main" << endl;
  process_queued_msg();
  return 0;
}
