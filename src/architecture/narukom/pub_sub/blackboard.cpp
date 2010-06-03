#include "blackboard.h"
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using google::protobuf::Message;
using boost::posix_time::ptime;
using  boost::posix_time::milliseconds;
using boost::make_tuple;
using std::string;

Blackboard::Blackboard(unsigned int cleanup_period): Subscriber()
{
  period = boost::posix_time::milliseconds(cleanup_period);
  cur_tmsp = boost::posix_time::microsec_clock::local_time();
}
Blackboard::Blackboard(const std::string& sub_name, unsigned int cleanup_period): Subscriber(sub_name)
{
  period = boost::posix_time::milliseconds(cleanup_period);
  cur_tmsp = boost::posix_time::microsec_clock::local_time();
}

Blackboard::Blackboard(const char* sub_name, unsigned int cleanup_period): Subscriber(sub_name)
{
  period = boost::posix_time::milliseconds(cleanup_period);
  cur_tmsp = boost::posix_time::microsec_clock::local_time();
}


void Blackboard::process_messages()
{

    MessageBuffer* sub_buf = Subscriber::getBuffer();
    if (sub_buf == 0)
        return;

    if (sub_buf == 0)
        cout << "None Unprocessed Buffers" << endl;
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    if ( (now - cur_tmsp ) > period)
    {
        cleanup();
        cur_tmsp = now;
    }

    Tuple* cur = sub_buf->remove_head();
    while (cur != 0 )
    {
        data_struct_mx.Lock();
        DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();
        default_index.insert(cur);
        data_struct_mx.Unlock();
        cur = sub_buf->remove_head();
    }
}

unsigned int Blackboard::get_cleanup_period()
{
  return period.total_milliseconds();
}
void Blackboard::set_cleanup_period(unsigned int cleanup_period)
{
  period = boost::posix_time::milliseconds(cleanup_period);
}

int Blackboard::add_tuple(Tuple* foo)
{
    DataStruct::iterator t1,t2;

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

    if(it == default_index.end())
    {
        data_struct_mx.Unlock();
        return world_perception.end();
    }
    Tuple* tmp = (*it);
    if (!(tmp->get_host() == host && tmp->get_publisher() == process && tmp->get_type() == type))
    {
        tmp = (*it);
        if ( it == default_index.end() ||  !(tmp->get_host() == host && tmp->get_publisher() == process && tmp->get_type() == type))
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
    tmp = (*it);
    if ( it != default_index.end() &&  tmp->get_host() == host && (tmp->get_publisher() == process) && (tmp->get_type() == type) )
    {
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
    }
    else
    {
      data_struct_mx.Unlock();
      return it2;
    }
    cout << "Blackboard: You should not read this  PLEASE DEBUG " << endl;
    data_struct_mx.Unlock();

    return world_perception.end();

}

DataStruct::iterator Blackboard::get_tuple(const std::string& host, const std::string&  process,const std::string& type)
{
    data_struct_mx.Lock();
    
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








int Blackboard::cleanup()
{
    DataStruct::nth_index<1>::type& time_out_index =  world_perception.get<1>();
    DataStruct::nth_index<1>::type::iterator it = time_out_index.begin();
    int i =0;
    if( it != time_out_index.end())
    {
        while (  it != time_out_index.end() && (*it)->get_timeout() < cur_tmsp )
        {
            time_out_index.erase(it);
            it = time_out_index.begin();
        }
    }
    return i;
}

