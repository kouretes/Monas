#include "blackboard.h"
#include <boost/date_time/posix_time/posix_time.hpp>
using google::protobuf::Message;
using boost::posix_time::ptime;
using boost::posix_time::milliseconds;
using std::string;




Blackboard::Blackboard(const std::string& sub_name): Subscriber(sub_name),Publisher(sub_name)
{

}


void Blackboard::process_messages()
{

    cleanup();


// 		cout << "SIZE OF BUFFER = " << buf->size() << endl;
//     _blk->process_messages();
//     cout << "After returns " << endl
    std::vector<msgentry> msg=Subscriber::remove();
    std::vector<msgentry>::iterator it=msg.begin();
    signalentry newsig;

   for(;it!=msg.end();++it)
    {

		size_t newtypeid=typeRegistry.registerNew((*it).msg->GetTypeName());
        switch ((*it).msgclass)
        {
			case msgentry::STATE:
            case msgentry::DATA:
                if(blkdata[newtypeid].size()>0)
                    blkdata[newtypeid].insert(--blkdata[newtypeid].end(),*it);//Suggest last place to add it
                else
                    blkdata[newtypeid].insert(*it);
                break;
            case msgentry::SIGNAL:

                newsig.d=(*it);
                newsig.cleared=false;
                sigdata[newtypeid]=newsig;

                break;
            /*
            case msgentry::STATE:
                statedata[(*it).msg->GetTypeName()]=(*it);
                break;
			*/

        }

        //cout<<(*it).msg->GetTypeName()<<":"<<blkdata[(*it).msg->GetTypeName()].size()<<endl;


    }



//      cout << "SIZE OF BLACKBOARD " << Subscriber::getName() << world_perception.size() << endl;
}



int Blackboard::cleanup()
{

    //Data structure
	std::map<std::size_t,historyqueue>::iterator it= blkdata.begin();
    //cout<<"cleanup2!"<<endl;
	boost::posix_time::time_duration t;
	boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
	while(it!=blkdata.end())
	{
	    historyqueue &q=(*it).second;

	    //cout<<(*it).first<<endl;
	    //int i=0;

	    if(blkdatatimeouts.find((*it).first)==blkdatatimeouts.end())
		{
			q.erase(q.begin(),--q.end());//Keep last only
		}
		else
		{
			boost::posix_time::ptime timeoutstamp=now-blkdatatimeouts[(*it).first];

			historyqueue::iterator qit= q.begin();
			while(qit!=q.end() && (*qit).timestamp<=timeoutstamp)
			{
				++qit;
				//i++;

			}
			//cout<<i<<endl;
			//q.clear();
			if(qit==q.end())--qit;
			q.erase(q.begin(),qit);

		}

	    ++it;
	}

	//Signal structure
	signalstruct::iterator sigit= sigdata.begin();
	while(sigit!=sigdata.end())
	{
	    if((*sigit).second.cleared==true)
	    {
	    	signalstruct::iterator t=sigit++;
	    	sigdata.erase(t);
	    	//sigit=sigdata.begin();
	    }
	    else
			++sigit;
	}

   return 0;

}


void Blackboard::publish_all()
{
    Publisher::publish(topublish);
    topublish.clear();

}

void Blackboard::publish_data(const google::protobuf::Message & msg,std::string const& topic)
{
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    //nmsg.timeoutstamp=now+boost::posix_time::millisec(timeout);
    nmsg.timestamp=now;
    nmsg.topic=topic;
    //nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::DATA;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
	std::size_t newtypeid=typeRegistry.registerNew(msg.GetTypeName());
    if(blkdata[newtypeid].size()>0)
        blkdata[newtypeid].insert(--blkdata[newtypeid].end(),nmsg);//Suggest last place to add it
    else
         blkdata[newtypeid].insert(nmsg);
    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;q

}

void Blackboard::publish_signal(const google::protobuf::Message & msg,std::string const& topic)
{
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    //nmsg.timeoutstamp=now;//Signal, no timeout
    nmsg.timestamp=now;
    nmsg.topic=topic;
    //nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::SIGNAL;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
    signalentry newsig;
    newsig.d=nmsg;
    newsig.cleared=false;
    sigdata[typeRegistry.registerNew(msg.GetTypeName())]=newsig;//If exists replace

    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;

}

void Blackboard::publish_state(const google::protobuf::Message & msg,std::string const& topic)
{
    publish_data(msg,topic);
    /*
    msgentry nmsg;

    google::protobuf::Message * newptr=msg.New();
    newptr->CopyFrom(msg);
    nmsg.msg.reset(newptr);

    //cout<<"In:"<<&msg;
    //cout<<"Copy:"<<nmsg.msg<<endl;
    nmsg.host="localhost";
    boost::posix_time::ptime now=boost::posix_time::microsec_clock::universal_time();
    //nmsg.timeoutstamp=now;//State, no timeout :)
    nmsg.timestamp=now;
    nmsg.topic=topic;
    //nmsg.publisher=Publisher::getName();
    nmsg.msgclass=msgentry::STATE;
    //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
    statedata[msg.GetTypeName()]=nmsg;//If exists replace

    topublish.push_back(nmsg);

    //cout<<blkdata[msg.GetTypeName()].size()<<endl;
    */

}



