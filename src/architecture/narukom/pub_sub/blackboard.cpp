#include "blackboard.h"
#include <boost/date_time/posix_time/posix_time.hpp>
using google::protobuf::Message;
using boost::posix_time::ptime;
using  boost::posix_time::milliseconds;
using std::string;




Blackboard::Blackboard(const std::string& sub_name): Subscriber(sub_name),Publisher(sub_name)
{

}

Blackboard::Blackboard(const char* sub_name): Subscriber(sub_name),Publisher(sub_name)
{

}


void Blackboard::process_messages()
{
    cur_tmsp = boost::posix_time::microsec_clock::universal_time();

    cleanup();

    MessageBuffer* buf = Subscriber::getBuffer();
		if(buf == 0)
			return;
// 		cout << "SIZE OF BUFFER = " << buf->size() << endl;
//     _blk->process_messages();
//     cout << "After returns " << endl
    std::vector<msgentry> msg=buf->remove();
    std::vector<msgentry>::iterator it=msg.begin();
    signalentry newsig;

   for(;it!=msg.end();++it)
    {


        switch ((*it).msgclass)
        {
            case msgentry::DATA:

                if(blkdata[(*it).msg->GetTypeName()].size()>0)
                    blkdata[(*it).msg->GetTypeName()].insert(--blkdata[(*it).msg->GetTypeName()].end(),*it);//Suggest last place to add it
                else
                    blkdata[(*it).msg->GetTypeName()].insert(*it);
                break;
            case msgentry::SIGNAL:

                newsig.d=(*it);
                newsig.cleared=false;
                sigdata[(*it).msg->GetTypeName()]=newsig;

                break;
            case msgentry::STATE:
                statedata[(*it).msg->GetTypeName()]=(*it);
                break;

        }

        //cout<<(*it).msg->GetTypeName()<<":"<<blkdata[(*it).msg->GetTypeName()].size()<<endl;


    }



//      cout << "SIZE OF BLACKBOARD " << Subscriber::getName() << world_perception.size() << endl;
}



int Blackboard::cleanup()
{

    //Data structure
	std::map<std::string,historyqueue>::iterator it= blkdata.begin();
    //cout<<"cleanup2!"<<endl;
	while(it!=blkdata.end())
	{
	    historyqueue &q=(*it).second;
	    historyqueue::iterator qit= q.begin();
	    //cout<<(*it).first<<endl;
	    //int i=0;
	    while(qit!=q.end() && (*qit).timeoutstamp<cur_tmsp)
	    {
            ++qit;
            //i++;

	    }
	    //cout<<i<<endl;
	    //q.clear();
	    q.erase(q.begin(),qit);

	    ++it;
	}

	//Signal structure
	signalstruct::iterator sigit= sigdata.begin();
	while(sigit!=sigdata.end())
	{
	    if((*sigit).second.cleared==true)
	    {
            sigdata.erase(sigit);
            sigit=sigdata.begin();
            continue;
	    }
	    ++sigit;
	}

   return 0;

}


void Blackboard::publish_all()
{
    Publisher::publish(topublish);
    topublish.clear();

}

void Blackboard::publish_data(const google::protobuf::Message & msg,std::string topic, int timeout)
{
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    nmsg.timeoutstamp=now+boost::posix_time::millisec(timeout);
    nmsg.timestamp=now;
    nmsg.topic=topic;
    nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::DATA;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
    if(blkdata[msg.GetTypeName()].size()>0)
        blkdata[msg.GetTypeName()].insert(--blkdata[msg.GetTypeName()].end(),nmsg);//Suggest last place to add it
    else
         blkdata[msg.GetTypeName()].insert(nmsg);
    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;q

}


void Blackboard::publish_signal(const google::protobuf::Message & msg,std::string topic)
{
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    nmsg.timeoutstamp=now;//Signal, no timeout
    nmsg.timestamp=now;
    nmsg.topic=topic;
    nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::SIGNAL;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
    signalentry newsig;
    newsig.d=nmsg;
    newsig.cleared=false;
    sigdata[msg.GetTypeName()]=newsig;//If exists replace

    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;

}

void Blackboard::publish_state(const google::protobuf::Message & msg,std::string topic)
{
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    nmsg.timeoutstamp=now;//State, no timeout :)
    nmsg.timestamp=now;
    nmsg.topic=topic;
    nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::STATE;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
    statedata[msg.GetTypeName()]=nmsg;//If exists replace

    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;

}



