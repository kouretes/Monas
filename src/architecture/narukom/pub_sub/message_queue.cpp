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
#include "tools/XML.h"


using std::map;
using std::string;
using namespace std;

MessageQueue::MessageQueue() : Thread(false)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
    create_tree();


}
void MessageQueue::addTopic(std::string const& what,std::string const& under)
{
	size_t underid=topicRegistry.getId(under);
	size_t newid=topicRegistry.getId(what);
	if(newid!=0)
		return;
	newid=topicRegistry.registerNew(what);
	topictree[underid].children.insert(newid);
	topictree[newid].parentid=underid;
}
void MessageQueue::create_tree()
{
	//cout << "Could not load file Default tree created" << file_name << endl;
	topictree[0].parentid=0;
	topictree[0].children.insert(topicRegistry.registerNew("global"));
	addTopic(string("motion"),string("global"));
	addTopic(string("leds"),string("global"));
	addTopic(string("sensors"),string("global"));
	addTopic(string("vision"),string("global"));
	addTopic(string("behavior"),string("global"));
	addTopic(string("localization"),string("global"));
	addTopic(string("communication"),string("global"));
	addTopic(string("obstacle"),string("global"));
}

void MessageQueue::purgeBuffer(MessageBuffer *b)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	std::map<std::size_t,std::set<MessageBuffer*> >::iterator mit=subscriptions.begin();
	for(;mit!=subscriptions.end();++mit)
	{
		(*mit).second.erase(b);
	}


}
MessageBuffer* MessageQueue::attachPublisher(std::string const& s)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid,*this,true);
    return new_msg_buf;
}

MessageBuffer* MessageQueue::attachSubscriber(std::string const& s)
{
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid,*this,false);//false siginifies no waking up to deliver these
    return new_msg_buf;
}


void MessageQueue::subscribeTo(MessageBuffer *b, std::string const& topic , int where)
{
	if(b==NULL)
		return;
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	size_t topicId=topicRegistry.getId(topic);
	//cout<<"Check 0"<<endl;
	if(topicId==0)
		return;
	//cout<<"Check 1"<<endl;
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicId);
	if(tit==topictree.end())
			return;
	//cout<<"Check 2"<<endl;
	subscriptions[topicId].insert(b);
	if(where==ABOVE_TOPIC||where==ALL)
	{

		do
		{
			tit=topictree.find((*tit).second.parentid);
			if(tit==topictree.end())
				break;
			subscriptions[(*tit).first].insert(b);
		}
		while((*tit).first!=(*tit).second.parentid);
	}

	if(where==BELOW_TOPIC||where==ALL)
		subscribeBelow(b,topicId);
}


void MessageQueue::subscribeBelow(MessageBuffer *b, size_t topicid )
{
	if(b==NULL)
		return;
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicid);
	if(tit==topictree.end())
			return ;
	subscriptions[topicid].insert(b);
	std::set<std::size_t>::const_iterator cit=(*tit).second.children.begin();
	for(;cit!=(*tit).second.children.end();++cit)//Recursive calls
		subscribeBelow(b,*cit);


}

void MessageQueue::unsubscribeFrom(MessageBuffer *b, std::string const& topic , int where)
{
	if(b==NULL)
		return;
	boost::unique_lock<boost::mutex > pub_sub_lock(pub_sub_mutex);
	size_t topicId=topicRegistry.getId(topic);
	if(topicId==0)
		return;
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicId);
	if(tit==topictree.end())
			return;
	subscriptions[topicId].erase(b);
	if(where==ABOVE_TOPIC||where==ALL)
	{

		do
		{
			tit=topictree.find((*tit).second.parentid);
			if(tit==topictree.end())
				break;
			subscriptions[(*tit).first].erase(b);
		}
		while((*tit).first!=(*tit).second.parentid);
	}

	if(where==BELOW_TOPIC||where==ALL)
		unsubscribeBelow(b,topicId);
}
void MessageQueue::unsubscribeBelow(MessageBuffer *b, size_t topicid )
{
	if(b==NULL)
		return;
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicid);
	if(tit==topictree.end())
			return ;
	subscriptions[topicid].erase(b);
	std::set<std::size_t>::const_iterator cit=(*tit).second.children.begin();
	for(;cit!=(*tit).second.children.end();++cit)//Recursive calls
		subscribeBelow(b,*cit);


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
			size_t msgtopicId=topicRegistry.getId((*mit).topic);
			//cout<<"Topic->id:"<<(*mit).topic<<msgtopicId<<endl;
			if(subscriptions.find(msgtopicId)==subscriptions.end())
				continue;
			std::set<MessageBuffer*>::const_iterator subit= subscriptions[msgtopicId].begin();
			//cout<<"Subscribers:"<<subscriptions[msgtopicId].size()<<endl;

			for ( ; subit!=subscriptions[msgtopicId].end(); ++subit)
			{
                      // cout<<"dest"<<endl;

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
