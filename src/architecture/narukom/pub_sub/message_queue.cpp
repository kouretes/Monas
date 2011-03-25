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
#include "message_buffer.h"
#include "topicTree.h"
#include "tools/XML.h"


using std::map;
using std::string;
using namespace std;

MessageQueue::MessageQueue() : Thread(false)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	tree=new TopicTree("global");
    create_tree();


}
/*
void MessageQueue::addTopic(std::string const& what,std::string const& under)
{
	tree->addTopic(what,under);

}*/
void MessageQueue::create_tree()
{
	//cout << "Could not load file Default tree created" << file_name << endl;
/*

	*/
	tree->addTopic(string("motion"),string("global"));
	tree->addTopic(string("leds"),string("global"));
	tree->addTopic(string("sensors"),string("global"));
	tree->addTopic(string("vision"),string("global"));
	tree->addTopic(string("behavior"),string("global"));
	tree->addTopic(string("localization"),string("global"));
	tree->addTopic(string("communication"),string("global"));
	tree->addTopic(string("obstacle"),string("global"));
	subscriptions.resize(tree->size());
}

void MessageQueue::purgeBuffer(MessageBuffer *b)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	std::vector< std::set<MessageBuffer*> >::iterator mit=subscriptions.begin();
	for(;mit!=subscriptions.end();++mit)
	{
		(*mit).erase(b);
	}


}
MessageBuffer* MessageQueue::attachPublisher(std::string const& s)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);
	new_msg_buf->setNotifier(boost::bind(&MessageQueue::requestMailMan,this,_1));

    return new_msg_buf;
}

MessageBuffer* MessageQueue::attachSubscriber(std::string const& s)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);
	new_msg_buf->setCleanUp(boost::bind(&MessageQueue::purgeBuffer,this,_1));
    return new_msg_buf;
}


void MessageQueue::subscribeTo(MessageBuffer *b, std::string const& topic , int where)
{
	if(b==NULL)
		return;
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	std::set<std::size_t> r=tree->iterateTopics(topic,where);

	std::set<std::size_t>::iterator tit=r.begin();

	for(;tit!=r.end();++tit)
	{
		//cout<<"sub:"<<*tit<<" "<< b<<endl;
		subscriptions[*tit].insert(b);
	}

}

void MessageQueue::unsubscribeFrom(MessageBuffer *b, std::string const& topic , int where)
{
	if(b==NULL)
		return;
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	std::set<std::size_t> r=tree->iterateTopics(topic,where);

	std::set<std::size_t>::iterator tit=r.begin();

	for(;tit!=r.end();++tit)
	{
		subscriptions[*tit].erase(b);
	}
}


void MessageQueue::process_queued_msg()
{
    std::vector<MessageBuffer *> toprocess;
    /* LOCKING */
    boost::unique_lock<boost::mutex > cond_lock(cond_mutex);
    while(cond_publishers.size()==0)
        cond.wait(cond_lock);
	toprocess=cond_publishers_queue;
	cond_publishers.clear();
	cond_publishers_queue.clear();
    cond_lock.unlock();
    /* LOCKING */
	// cout<<"Queue up!"<<endl;

    //boost::unique_lock<boost::mutex > sub_lock(sub_mutex)
	static int _executions = 0;
	static int msgs=0;
	_executions ++;
    agentStats.StartTiming();
    for(std::vector<MessageBuffer *>::iterator pit=toprocess.begin();pit!=toprocess.end();++pit)
    {

        std::vector<msgentry> mtp=(*pit)->remove();
        std::map<MessageBuffer *,std::vector<msgentry> > ready;
        //cout <<(*pit)->getOwnerID() << ":"<<mtp.size() << endl;
        const std::size_t pownerid=(*pit)->getOwnerID();
		boost::unique_lock<boost::mutex > pub_sub_mutexlock(pub_sub_mutex);
        for(std::vector<msgentry>::iterator mit=mtp.begin();mit!=mtp.end();++mit)
        {
			size_t msgtopicId=tree->getId((*mit).topic);
			//cout<<"Topic->id:"<<(*mit).topic<<msgtopicId<<" "<<subscriptions.size()<<endl;
			if(msgtopicId==0||subscriptions.size()<=msgtopicId)
				continue;
			std::set<MessageBuffer*>::const_iterator subit= subscriptions[msgtopicId].begin();
			//cout<<"Subscribers:"<<subscriptions[msgtopicId].size()<<endl;

			for ( ; subit!=subscriptions[msgtopicId].end(); ++subit)
			{
				//cout<<"dest"<<endl;
				if ((*subit)->getOwnerID() != pownerid  )
				{
					//cout << "Delivering to " << (*subit)->getOwnerID()<< " the " << (*mit).msg->GetTypeName() << " size: " << endl;
					ready[(*subit)].push_back(*mit);
					msgs++;
				}
			}
        }
        pub_sub_mutexlock.unlock();

        /*if ( ! (_executions % 1000) ){
			for(std::map<MessageBuffer *,std::vector<msgentry> >::iterator rit=ready.begin();rit!=ready.end();++rit)
			{
				cout<<":"<<(*rit).second.size()<<endl;
			}
        }*/
		std::map<MessageBuffer *,std::vector<msgentry> >::iterator rit=ready.end();
		while(ready.size()>0)
		{
			if(rit==ready.end())
				rit=ready.begin();
			if((*rit).first->tryadd((*rit).second))
			{
				std::map<MessageBuffer *,std::vector<msgentry> >::iterator t=rit++;
				ready.erase(t);

			}
			else
				rit++;
		}



    }
    if ( ! (_executions % 10000) ){
                 cout << "Narukom time " << agentStats.StopTiming()/msgs << endl;
                 _executions=0;
                 msgs=0;
             }
		else
			agentStats.StopTiming();


}


//void MessageQueue::finalize_queue(){}
int MessageQueue::Execute()
{
	process_queued_msg();
	return 0;
}
