#include "blackboard.h"
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using google::protobuf::Message;
using boost::posix_time::ptime;
using  boost::posix_time::milliseconds;
using boost::make_tuple;
using std::string;


list< string >* host_publisher_index::get_hosts() const
{
 std::list<std::string>* host_list = new list<std::string>();
 for( std::map<std::string,std::list<std::string>* >::const_iterator it = hosts.begin(); it != hosts.end(); it++)
 {
	 host_list->push_back(it->first);
 }
 return host_list;
}

list< string >* host_publisher_index::get_publishers(const std::string& host) const
{
	std::list<std::string>* pub_list = 0;
	std::map<std::string,std::list<std::string>* >::const_iterator it = hosts.find(host);
	if(it != hosts.end())
		pub_list = it->second;
	return pub_list;
}
bool host_publisher_index::has_publisher(const std::string& host, const std::string& pub) const
{
	std::map<std::string,std::list<std::string>* >::const_iterator it = hosts.find(host);
	if(it != hosts.end())
	{
		for(std::list<std::string>::iterator pub_it = (it->second)->begin(); pub_it != it->second->end(); it++)
			if((*pub_it) == pub)
				return true;
	}
	return false;
}
bool host_publisher_index::host_exists(const std::string& host) const
{
	std::map<std::string,std::list<std::string>* >::const_iterator it = hosts.find(host);
	if(it != hosts.end())
		return true;
	return false;
}

bool host_publisher_index::insert_pair(const std::string& host, const std::string& publisher)
{
	std::map<std::string,std::list<std::string>* >::iterator it = hosts.find(host);
	if(it != hosts.end())
	{
		if(!has_publisher(host,publisher))
		{
			it->second->push_back(publisher);
			return true;
		}
	}
	return false;
}

bool host_publisher_index::remove_pair(const std::string& host, const std::string& publisher)
{
	std::map<std::string,std::list<std::string>* >::iterator it = hosts.find(host);
	if(it != hosts.end())
	{
		for(std::list<std::string>::iterator pub_it = (it->second)->begin(); pub_it != it->second->end(); it++)
			if((*pub_it) == publisher)
			{
				it->second->erase(pub_it);
				return true;
			}
	}
	return false;
}


list< string >* Blackboard::get_hosts() const
{
	return hosts.get_hosts();
}

list< string >* Blackboard::get_publishers(const std::string& host) const
{
	return hosts.get_publishers(host);  
}


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
		if (sub_buf->size() < 1)
			return;
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

		std::vector<Tuple*> tmp_v;
    unsigned int s = 0;
		if ( (now - cur_tmsp ) > period)
    {
				cur_tmsp = now;
        cleanup();
    }    
    while (sub_buf->size() > 0 )
    {
        tmp_v.push_back(sub_buf->remove_tail());
				
    }

    while(s < tmp_v.size())
		{
// 			cout << "timeout " << tmp_v[s]->get_timeout() << endl;
			world_perception.insert(tmp_v[s]);
			s++;
		}
//      cout << "SIZE OF BLACKBOARD " << Subscriber::getName() << world_perception.size() << endl;
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
    world_perception.insert(foo);
    return world_perception.size();
}
DataStruct::iterator Blackboard::get_tuple(const std::string& host, const std::string&  process,const std::string& type ,boost::posix_time::ptime* time_req)
{
    DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();
    DataStruct::iterator it = default_index.upper_bound(make_tuple(host,process,type,*time_req));

    if(it == default_index.end())
    {
        
        return world_perception.end();
    }
    Tuple* tmp = (*it);
    if (!(tmp->get_host() == host && tmp->get_publisher() == process && tmp->get_type() == type))
    {
        tmp = (*it);
        if ( it == default_index.end() ||  !(tmp->get_host() == host && tmp->get_publisher() == process && tmp->get_type() == type))
        {
        
            return world_perception.end();
        }
        
        return it;
    }
    if ( (*it)->get_timestamp() == *time_req)
    {
        
        return it;
    }
    DataStruct::iterator it2 = it;
    it--;
    tmp = (*it);
    if ( it != default_index.end() &&  tmp->get_host() == host && (tmp->get_publisher() == process) && (tmp->get_type() == type) )
    {
        if (((*it2)->get_timestamp() - *time_req) < (*time_req - (*it)->get_timestamp() ) )
        {
        
            return it2;

        }
        else
        {
        
            return it;
        }
    }
    else
    {
      
      return it2;
    }
    cout << "Blackboard: You should not read this  PLEASE DEBUG " << endl;
    

    return world_perception.end();

}

DataStruct::iterator Blackboard::get_tuple(const std::string& host, const std::string&  process,const std::string& type)
{

    
    DataStruct::nth_index<0>::type& default_index = world_perception.get<0>();
    {
        DataStruct::iterator lower,upper;
        boost::tuples::tie(lower,upper) = default_index.equal_range(make_tuple(host,process,type));

        if (lower != upper)
        {
            upper--;
            return upper;
        }
        else
        {
            return world_perception.end();
        }
    }
}
 DataStruct::nth_index<2>::type::iterator Blackboard::get_msg(const std::string& type, const std::string& host, boost::posix_time::ptime* time_req)
{
    DataStruct::nth_index<2>::type& default_index = world_perception.get<2>();
     DataStruct::nth_index<2>::type::iterator it = default_index.upper_bound(make_tuple(host,type,*time_req));

    if(it == default_index.end())
    {

        return default_index.end();
    }
    Tuple* tmp = (*it);
    if (!(tmp->get_host() == host && tmp->get_type() == type))
    {
        tmp = (*it);
        if ( it == default_index.end() ||  !(tmp->get_host() == host && tmp->get_type() == type))
        {

            return default_index.end();
        }

        return it;
    }
    if ( (*it)->get_timestamp() == *time_req)
    {

        return it;
    }
     DataStruct::nth_index<2>::type::iterator it2 = it;
    it--;
    tmp = (*it);
    if ( it != default_index.end() &&  tmp->get_host() == host && (tmp->get_type() == type) )
    {
        if (((*it2)->get_timestamp() - *time_req) < (*time_req - (*it)->get_timestamp() ) )
        {

            return it2;

        }
        else
        {

            return it;
        }
    }
    else
    {

      return it2;
    }
    cout << "Blackboard: You should not read this  PLEASE DEBUG " << endl;


    return default_index.end();
}

 DataStruct::nth_index<2>::type::iterator Blackboard::get_msg(const std::string& type, const std::string& host)
{
    DataStruct::nth_index<2>::type& default_index = world_perception.get<2>();
    {
        DataStruct::nth_index<2>::type::iterator lower,upper;
        boost::tuples::tie(lower,upper) = default_index.equal_range(make_tuple(host,type));

        if (lower != upper)
        {
            upper--;
            return upper;
        }
        else
        {
            return default_index.end();
        }
    }
}

int Blackboard::cleanup()
{

	DataStruct::nth_index<1>::type& time_out_index =  world_perception.get<1>();
    DataStruct::nth_index<1>::type::iterator it = time_out_index.begin();
    int i =0;
		Tuple* t = 0;
// 		cout << "Inside cleanup " << endl;
    if( it != time_out_index.end())
    {
// 			cout << "if cleanup " << endl;
// 			cout << "timeout " << (*it)->get_timeout() << " -----  " << cur_tmsp << endl;
        while (  it != time_out_index.end() && (*it)->get_timeout() < cur_tmsp )
        {
// 						cout << " CLEANUP  " << --i << endl;
						t = (*it);
            time_out_index.erase(it);
						delete t;
            it = time_out_index.begin();
        }
    }
//     cout << "finished cleanup " << endl;
    return i;
}

