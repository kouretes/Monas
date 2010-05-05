#include "blackboard.h"
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using google::protobuf::Message;
using boost::posix_time::ptime;
using  boost::posix_time::milliseconds;
using boost::make_tuple;
using std::string;


//Agent::Agent(): agent_name("Default Agent"), Publisher(agent_name), Thread(false) {} //{blk  = new Blackboard(this);}
//Agent::Agent(std::string name) : agent_name(name), Publisher(agent_name), Thread(false) {}// {blk = new Blackboard(this);}
//Agent::Agent(const char* name ) : agent_name(name) , Thread( false)
//{
//    cout << "Agent Constructor " << endl;
//    Publisher(agent_name) ;
//    blk = new Blackboard(this);
//}
//void Agent::run() {
//
//    cout << "Agent RRRRRRRRRRRUUUUUUUUUUUUUUUUUUUUUN " << endl;
//
//    static bool flag = true;
//    if (blk != NULL)
//        blk->process_messages();
//    else
//        cout << "Not Initialized " << endl;
//    BasicMessage* mb = (BasicMessage*)blk->read_nb("BasicMessage","Trafficer","localhost");
//    if (mb && flag)
//    {
//        cout << "From Blackboard no block" << mb->topic() << endl;
//        //mb = (BasicMessage*)blk->in_nb("BasicMessage","Trafficer","localhost");
//        int i ;
//        //std::cin >> i ;
//        flag = false;
//        cout << "From Blackboard inno block" << mb->topic() << endl;
//    }
//    else
//    {
//        cout << "adjflkajsdlfkjasldkfjalskdfjla;skjdf;laskjfd;alskjfd;alkjfd;lakjf;alkjf;laskjdf;laksjdfl;aksjfd;laskjl;fdkja;lksdf << " << endl;
//        mb = (BasicMessage*)blk->in("BasicMessage","Trafficer","localhost");
//        cout << "From Blackboard  block" << mb->topic() << endl;
//        cout << "adjflkajsdlfkjasldkfjalskdfjla;skjdf;laskjfd;alskjfd;alkjfd;lakjf;alkjf;laskjdf;laksjdfl;aksjfd;laskjl;fdkja;lksdf << " << endl;
//
//    }
//
//}
Blackboard::Blackboard() : Subscriber(), myagent(NULL)
{

		period = boost::posix_time::milliseconds(100);
	cur_tmsp = boost::posix_time::microsec_clock::local_time();
//	int i;
//	std::cin >> i;
//	cout << "asfddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd" << endl;
}

Blackboard::Blackboard(const string& name) : Subscriber(name), myagent(NULL)
{
	period = boost::posix_time::milliseconds(100);
	cur_tmsp = boost::posix_time::microsec_clock::local_time();
//	int i;
//
//	cout << "asfddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd" << endl;
}
Blackboard::Blackboard(const char* name) : Subscriber(name), myagent(NULL)
{


}
//Blackboard::Blackboard(Agent* agent ): Subscriber(agent->get_name()), myagent(agent) {
//
//
//}
//     BlBlackboard(const Subscriber& other);
//Blackboard::Blackboard(const Blackboard& other): Subscriber(Subscriber(other)),myagent(other.getAgent())
//{
//    //TODO
//    //message_pool.copyFrom(other.getMessagePool())	period = boost::posix_time::milliseconds(100);
//
//
//}

void Blackboard::process_messages()
{

    MessageBuffer* sub_buf = Subscriber::getBuffer();
    if (sub_buf == NULL)
        return;

    if (sub_buf == NULL)
        cout << "None Unprocessed Buffers" << endl;
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		if( (now - cur_tmsp ) > period)
		{
			cleanup();
			cur_tmsp = now;
		}
		
    google::protobuf::Message* cur = sub_buf->remove_head();
    while (cur != NULL )
    {
        Tuple* t = new Tuple(cur);
        data_struct_mx.Lock();
        DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();

        default_index.insert(t);
        data_struct_mx.Unlock();
        cur = sub_buf->remove_head();
    }
}
int Blackboard::add_tuple(Tuple* foo)
{
    DataStruct::iterator t1,t2;
//	cout << "host " << foo->get_host() << " pub: " << foo->get_publisher() <<" type:" <<   foo->get_type() << " tm: "<< foo->get_timestamp() << endl;

    // world_perception.get<0>().insert(foo);
    data_struct_mx.Lock();
    world_perception.insert(foo);
    data_struct_mx.Unlock();
    return world_perception.size();

}
DataStruct::iterator Blackboard::get_tuple(const std::string& host, const std::string&  process,const std::string& type ,boost::posix_time::ptime* time_req)
{
    data_struct_mx.Lock();
    DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();
    DataStruct::iterator it = default_index.upper_bound(make_tuple(host,process,type,*time_req));

    BasicMessage * tmp,*tmp2 ;
    if (it != default_index.end())
        tmp = ((BasicMessage*)(*it)->msg);
    else
    {
        data_struct_mx.Unlock();
        return world_perception.end();
    }
    //cout << " NOT END " << endl;
    if (!(tmp->host() == host && tmp->publisher() == process && tmp->GetTypeName() == type))
    {
        it--;
        tmp = ((BasicMessage*)(*it)->msg);
        if (!(tmp->host() == host && tmp->publisher() == process && tmp->GetTypeName() == type))
        {
            data_struct_mx.Unlock();
            return world_perception.end();
        }
        data_struct_mx.Unlock();
        return it;
    }
    if ( (*it)->get_timestamp() == *time_req)
    {
        data_struct_mx.Unlock();
        return it;
    }
    DataStruct::iterator it2 = it;
    it--;
    tmp = ((BasicMessage*)(*it)->msg);
    if (tmp->host() == host && (tmp->publisher() == process) && (tmp->GetTypeName() == type) )
        if (((*it2)->get_timestamp() - *time_req) < (*time_req - (*it)->get_timestamp() ) )
        {
            data_struct_mx.Unlock();
            return it2;

        }
        else
        {
            data_struct_mx.Unlock();
            return it;
        }
    cout << "Blackboard: You should not read this  PLEASE DEBUG " << endl;
    data_struct_mx.Unlock();

// 	 return it;

}

DataStruct::iterator Blackboard::get_tuple(const std::string& host, const std::string&  process,const std::string& type)
{
    data_struct_mx.Lock();
    Message* result =0;
    DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();
    {
        DataStruct::iterator lower,upper;
        boost::tuples::tie(lower,upper) = default_index.equal_range(make_tuple(host,process,type));

        if (lower != upper)
        {
            upper--;
            data_struct_mx.Unlock();
            return upper;
        }
        else
        {
            data_struct_mx.Unlock();
            return world_perception.end();
        }
    }


}
Message* Blackboard::read(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req)
{
    Message* result =0;
    DataStruct::iterator data_it;
    if (time_req == 0)
    {

        data_it = get_tuple(host,process,type);
        while (data_it == world_perception.end())
        {
            boost::thread::yield();
						process_messages();
            data_it = get_tuple(host,process,type);
        }
    }
    else
    {
        data_it = get_tuple(host,process,type,time_req);
        while (data_it == world_perception.end())
        {
            boost::thread::yield();
						process_messages();
            data_it = get_tuple(host,process,type,time_req);
        }

    }
    result = (*data_it)->msg;
    return result;
}

Message* Blackboard::in(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req)
{
    Message* result =0;
    DataStruct::iterator data_it;
    if (time_req == 0)
    {
        data_it = get_tuple(host,process,type);
        while (data_it == world_perception.end())
        {
            boost::thread::yield();
						process_messages();
            data_it = get_tuple(host,process,type);
// 			cout << "RUNS" << endl;
        }
    }
    else
    {
        data_it = get_tuple(host,process,type,time_req);
        while (data_it == world_perception.end())
        {
            boost::thread::yield();
						process_messages();
            data_it = get_tuple(host,process,type,time_req);
        }

    }
    result = (*data_it)->msg;
    data_struct_mx.Lock();
    world_perception.erase(data_it);
    data_struct_mx.Unlock();


    return result;
}

Message* Blackboard::read_nb(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req)
{
    Message* result =0;
    DataStruct::iterator data_it;
    if (time_req == 0)
    {
        data_it = get_tuple(host,process,type);
    }
    else
    {
        data_it = get_tuple(host,process,type,time_req);
    }
    if (data_it != world_perception.end())
        result = (*data_it)->msg;
    return result;
}



Message* Blackboard::in_nb(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req)
{
    Message* result =0;
    DataStruct::iterator data_it;
    if (time_req == 0)
    {
        data_it = get_tuple(host,process,type);
    }
    else
    {
        data_it = get_tuple(host,process,type,time_req);
    }


    if (data_it != world_perception.end())
    {
        result = (*data_it)->msg;
        data_struct_mx.Lock();
        world_perception.erase(data_it);
        data_struct_mx.Unlock();
    }
    return result;
}

int Blackboard::cleanup()
{
	DataStruct::nth_index<1>::type& time_out_index =  world_perception.get<1>();
 	DataStruct::nth_index<1>::type::iterator it = time_out_index.begin();
	//cout << "Cleaning " << endl;
	int i =0;
	if( it != time_out_index.end())
	{
	//	cout << "INF " << endl;
		while(  it != time_out_index.end() && (*it)->get_timeout() < cur_tmsp )
		{
			cout << "As of " << i++ << endl;
			time_out_index.erase(it);
			it = time_out_index.begin();
		}
	}
	else{
	//	cout << "AAAA" << endl;
	}
	//cout << "FOO" << endl;
	return 0; //Alex
}
