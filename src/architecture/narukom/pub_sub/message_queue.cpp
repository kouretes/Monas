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
#include "network/multicastpoint.hpp"
#include "topicTree.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"




using std::map;
using std::string;
using namespace std;
using namespace KSystem;

MessageQueue::MessageQueue() : Thread(false),
multicast(NULL),pubsubRegistry(),pub_sub_mutex(),subscriptions(),publisherbuffers(),subscriberBuffers(),
cond_mutex(),cond_publishers(),cond_publishers_queue(),cond(),agentStats()
{
	subscriptions.resize(Topics::Instance().size()+1);//Force Generation of instance and resize subscription vector

    XMLConfig xmlconfig(ArchConfig::Instance().GetConfigPrefix()+"/network.xml");
    string multicastip="";
    unsigned int port=0;
    unsigned maxpayload=0;
    unsigned beacon_interval=0;
    if(xmlconfig.QueryElement("multicast_ip", multicastip) &&
	   xmlconfig.QueryElement("multicast_port", port) &&
	   xmlconfig.QueryElement("maxpayload", maxpayload) &&
	   xmlconfig.QueryElement("beacon_interval", beacon_interval) )
	{
		cout<<"Initiating multicast network at address: "<<multicastip<<":"<<port<<std::endl;
		KNetwork::MulticastPoint *m=new KNetwork::MulticastPoint(multicastip,maxpayload);
		multicast=NULL;
		m->setCleanupAndBeacon(beacon_interval);
		m->attachTo(*this);
		if(m->startEndPoint(multicastip,port)==false)
			delete m;
		else
			multicast=m;
	}

}

MessageQueue::~MessageQueue()
{
		delete multicast;
}
/*
void MessageQueue::addTopic(std::string const& what,std::string const& under)
{
	tree->addTopic(what,under);

}*/

void MessageQueue::purgeBuffer(MessageBuffer *b)
{
	if(b==NULL)
			return;
	Mutex::scoped_lock pub_sub_lock(pub_sub_mutex);
	std::vector< std::set<MessageBuffer*> >::iterator mit=subscriptions.begin();
	for(;mit!=subscriptions.end();++mit)
	{
		(*mit).erase(b);
	}
	subscriberBuffers[b->getOwnerID()].erase(b);
	publisherbuffers[b->getOwnerID()].erase(b);


}
MessageBuffer* MessageQueue::makeWriteBuffer(std::string const& s)
{
	Mutex::scoped_lock pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	publisherbuffers.resize(pubsubRegistry.size()+1);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);
	new_msg_buf->setNotifier(boost::bind(&MessageQueue::requestMailMan,this,_1));
	new_msg_buf->setCleanUp(boost::bind(&MessageQueue::purgeBuffer,this,_1));
	publisherbuffers[newid].insert(new_msg_buf);

    return new_msg_buf;
}

MessageBuffer* MessageQueue::makeReadBuffer(std::string const& s)
{
	Mutex::scoped_lock pub_sub_lock(pub_sub_mutex);

	size_t newid=pubsubRegistry.registerNew(s);
	subscriberBuffers.resize(pubsubRegistry.size()+1);
	MessageBuffer* new_msg_buf = new MessageBuffer ( newid);
	new_msg_buf->setCleanUp(boost::bind(&MessageQueue::purgeBuffer,this,_1));
	subscriberBuffers[newid].insert(new_msg_buf);
    return new_msg_buf;
}


void MessageQueue::subscribeTo(std::size_t subid, std::size_t topic , int where)
{


	std::set<std::size_t> r=Topics::Instance().iterateTopics(topic,where);

	std::set<std::size_t>::iterator tit=r.begin();
	std::set<MessageBuffer *> & bset=subscriberBuffers[subid];
	std::set<MessageBuffer *>::iterator bit;

	for(;tit!=r.end();++tit)
	{
        //cout<<"sub:"<<*tit<<" "<< subid<<endl;
		for(bit=bset.begin();bit!=bset.end();++bit)
			subscriptions[*tit].insert(*bit);
	}

}

void MessageQueue::unsubscribeFrom(std::size_t subid, std::size_t topic , int where)
{

	std::set<std::size_t> r=Topics::Instance().iterateTopics(topic,where);

	std::set<std::size_t>::iterator tit=r.begin();

	std::set<MessageBuffer *> & bset=subscriberBuffers[subid];
	std::set<MessageBuffer *>::iterator bit;

	for(;tit!=r.end();++tit)
	{
		for(bit=bset.begin();bit!=bset.end();++bit)
			subscriptions[*tit].erase(*bit);
	}
}


void MessageQueue::process_queued_msg()
{
    std::vector<MessageBuffer *> toprocess;
    /* LOCKING */
    Mutex::scoped_lock cond_lock(cond_mutex);
    while(cond_publishers.size()==0)
        cond.wait(cond_lock);
	toprocess=cond_publishers_queue;
	cond_publishers.clear();
	cond_publishers_queue.clear();
    cond_lock.unlock();
    /* LOCKING */
	// cout<<"Queue up!"<<endl;

    //Mutex::scoped_lock sub_lock(sub_mutex)
	static int _executions = 0;
	static int msgs=0;
	_executions ++;
    agentStats.StartTiming();
     std::map<MessageBuffer *,std::vector<msgentry> > ready;
    for(std::vector<MessageBuffer *>::iterator pit=toprocess.begin();pit!=toprocess.end();++pit)
    {

        std::vector<msgentry> mtp=(*pit)->remove();

       // cout <<(*pit)->getOwnerID() << ":"<<mtp.size() << endl;
        const std::size_t pownerid=(*pit)->getOwnerID();
		Mutex::scoped_lock pub_sub_mutexlock(pub_sub_mutex);
        for(std::vector<msgentry>::iterator mit=mtp.begin();mit!=mtp.end();++mit)
        {
			size_t msgtopicId=(*mit).topic;


			//===== Handle subscriptions
			if((*mit).msgclass>=msgentry::SUBSCRIBE_ON_TOPIC && (*mit).msgclass<=msgentry::SUBSCRIBE_ALL_TOPIC)
			{
				subscribeTo(pownerid,(*mit).topic,(*mit).msgclass);
				if((*mit).host!=msgentry::HOST_ID_LOCAL_HOST&&multicast!=NULL)
				    multicast->getReadBuffer()->add((*mit));
				continue;
			}
			else if((*mit).msgclass>=msgentry::UNSUBSCRIBE_ON_TOPIC && (*mit).msgclass<=msgentry::UNSUBSCRIBE_ALL_TOPIC)
			{
				unsubscribeFrom(pownerid,(*mit).topic,(*mit).msgclass);
				if((*mit).host!=msgentry::HOST_ID_LOCAL_HOST&&multicast!=NULL)
					multicast->getReadBuffer()->add((*mit));
				continue;
			}


			//cout<<"Topicid:"<<msgtopicId<<" "<<subscriptions[msgtopicId].size()<<" "<<(*mit).msgclass<<endl;
			if(msgtopicId==0||subscriptions.size()<=msgtopicId)
				continue;
			std::set<MessageBuffer*>::const_iterator subit= subscriptions[msgtopicId].begin();
			//cout<<"Subscribers:"<<subscriptions[msgtopicId].size()<<endl;

			for ( ; subit!=subscriptions[msgtopicId].end(); ++subit)
			{
				//cout<<"dest"<<endl;
				if ((*subit)->getOwnerID() != pownerid  )
				{
					//cout << "Delivering to " <<  pubsubRegistry.getString((*subit)->getOwnerID())<< " the " << (*mit).msg->GetTypeName() << " size: " << endl;
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




    }

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
