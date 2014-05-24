#include "Blackboard.hpp"
#include "core/architecture/time/SystemTime.hpp"

namespace Messaging
{
    using google::protobuf::Message;
    using namespace KSystem::Time;
    using std::string;



    Blackboard::Blackboard(const std::string& sub_name): EndPoint(sub_name)
    {
    }


    void Blackboard::process_messages()
    {
        cleanup();
        // 		cout << "SIZE OF BUFFER = " << buf->size() << endl;
        //     _blk.process_messages();
        //     cout << "After returns " << endl
        std::vector<MessageEntry> msg = EndPoint::remove();
        std::vector<MessageEntry>::iterator it = msg.begin();
        signalentry newsig;
        brecord nrec;

        for(; it != msg.end(); ++it)
        {
            //std::cout<<"incoming:"<<(*it).msg->GetTypeName()<<(*it).msgclass<<" "<<MessageEntry::DATA<<std::endl;
            type_t newtypeid = typeRegistry.registerNew((*it).msg->GetTypeName());
            region_index i;
            i.tid = (*it).topic;
            i.hid = (*it).host;
            //std::cout<<"Incoming region:"<<i.hid<<" "<<i.tid<<"<size:"<<allrecords.size()<<std::endl;
            disjoint_region &r = allrecords[i];
            nrec.msg = (*it).msg;
            nrec.timestamp =SystemTime::unwrap((*it).timestamp);

            switch ((*it).msgclass)
            {
            case MessageEntry::STATE:
                r.blkstate[newtypeid] = nrec;
                break;

            case MessageEntry::DATA:

                //std::cout<<(*it).msg->GetTypeName()<<i.hid<<std::endl;
                if(r.blkdata[newtypeid].size() > 0)
                    r.blkdata[newtypeid].insert(--r.blkdata[newtypeid].end(), nrec); //Suggest last place to add it
                else
                    r.blkdata[newtypeid].insert(nrec);

                break;

            case MessageEntry::SIGNAL:
                newsig.d = nrec;
                newsig.cleared = false;
                r.blksignal[newtypeid] = newsig;
                break;

            default:
                break;
                /*
                case MessageEntry::STATE:
                    statedata[(*it).msg->GetTypeName()]=(*it);
                    break;
                */
            }

            //std::cout<<"Blackboard:"<<(*it).msgclass<<" "<<(*it).msg->GetTypeName()<<std::endl;
        }

        //      cout << "SIZE OF BLACKBOARD " << Subscriber::getName() << world_perception.size() << endl;
    }



    int Blackboard::cleanup()
    {
        regions::iterator rit = allrecords.begin();
        //KSystem::Time::TimeDuration t;
        KSystem::Time::TimeAbsolute now = SystemTime::now();

        for(; rit != allrecords.end(); ++rit)
        {
            disjoint_region &r = (*rit).second;
            //Data Structure
            datastruct::iterator it = r.blkdata.begin();

            for(; it != r.blkdata.end(); ++it)
            {
                recordlist &q = (*it).second;

                if(r.blkdatatimeouts.find((*it).first) == r.blkdatatimeouts.end())
                {
                    if(q.size() > 1 )q.erase(q.begin(), --q.end()); //Keep last only
                }
                else
                {
                    KSystem::Time::TimeAbsolute timeoutstamp = now - r.blkdatatimeouts[(*it).first];
                    brecord ar;
                    ar.timestamp = timeoutstamp;
                    recordlist::iterator qit = q.upper_bound(ar);

                    //cout<<i<<endl;
                    //q.clear();
                    std::cout<<(*qit).timestamp.toString()<<std::endl;
                    std::cout<<(*qit).msg->GetTypeName()<<std::endl;
                    if(qit == q.end() && q.size() > 0)--qit;

                    q.erase(q.begin(), qit);
                }
            }

            //Signal structure
            signalstruct::iterator sigit = r.blksignal.begin();

            while(sigit != r.blksignal.end())
            {
                if((*sigit).second.cleared == true)
                {
                    signalstruct::iterator t = sigit++;
                    r.blksignal.erase(t);
                    //sigit=sigdata.begin();
                }
                else
                    ++sigit;
            }
        }

        return 0;
    }


    void Blackboard::publish_all()
    {
        EndPoint::publish(topublish);
        topublish.clear();
    }

    void Blackboard::publishData(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg, test;
        brecord  nrec;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        nrec.msg = nmsg.msg;
        //std::cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        TimeAbsolute now =SystemTime::now();
        //nmsg.timeoutstamp=now+KSystem::Time::millisec(timeout);
        nmsg.timestamp = now.wrapTo<TimeStamp>();
        nrec.timestamp = now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        //std::cout<<"Blacboard publish data:"<<msg.GetTypeName()<<std::endl;
        nmsg.msgclass = MessageEntry::DATA;
        type_t newtypeid = typeRegistry.registerNew(msg.GetTypeName());
        region_index i;
        i.tid = nmsg.topic;
        i.hid = nmsg.host;
        disjoint_region &r = allrecords[i];

        if(r.blkdata[newtypeid].size() > 0)
            r.blkdata[newtypeid].insert(--r.blkdata[newtypeid].end(), nrec); //Suggest last place to add it
        else
            r.blkdata[newtypeid].insert(nrec);

        test = nmsg;
        //std::cout<<test.msgclass<<" "<<nmsg.msgclass<<std::endl;
        topublish.push_back(nmsg);
        //cout<<blkdata[msg.GetTypeName()].size()<<endl;q
    }

    void Blackboard::publishSignal(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg;
        brecord  nrec;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        nrec.msg = nmsg.msg;
        //cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        TimeAbsolute now =SystemTime::now();
        //nmsg.timeoutstamp=now;//Signal, no timeout
        nmsg.timestamp = now.wrapTo<TimeStamp>();
        nrec.timestamp =  now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        nmsg.msgclass = MessageEntry::SIGNAL;
        //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
        type_t newtypeid = typeRegistry.registerNew(msg.GetTypeName());
        region_index i;
        i.tid = nmsg.topic;
        i.hid = nmsg.host;
        disjoint_region &r = allrecords[i];
        signalentry newsig;
        newsig.d = nrec;
        newsig.cleared = false;
        r.blksignal[newtypeid] = newsig; //If exists replace
        topublish.push_back(nmsg);
        //cout<<blkdata[msg.GetTypeName()].size()<<endl;
    }

    void Blackboard::publishState(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg;
        brecord  nrec;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        nrec.msg = nmsg.msg;
        //cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        TimeAbsolute now =SystemTime::now();
        //nmsg.timeoutstamp=now;//State, no timeout :)
        nmsg.timestamp = now.wrapTo<TimeStamp>();
        nrec.timestamp =  now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        nmsg.msgclass = MessageEntry::STATE;
        //cout<<msg.GetTypeName()<<":"<<blkdata[msg.GetTypeName()].size()<<endl;
        type_t newtypeid = typeRegistry.registerNew(msg.GetTypeName());
        region_index i;
        i.tid = nmsg.topic;
        i.hid = MessageEntry::HOST_ID_LOCAL_HOST;
        disjoint_region &r = allrecords[i];
        r.blkstate[newtypeid] = nrec; //If exists replace
        topublish.push_back(nmsg);
        //cout<<blkdata[msg.GetTypeName()].size()<<endl;
    }
}


