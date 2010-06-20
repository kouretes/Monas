/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "subscriber.h"
#include "../system/Mutex.h"
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/asio.hpp>
#include <boost/concept_check.hpp>
#include "tuple.h"
#include <map>
#include <vector>
using namespace boost::multi_index;

typedef multi_index_container<
Tuple*,
indexed_by<
ordered_unique< standard_key
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
,composite_key_result_less<standard_key::result_type>
#endif
>,ordered_non_unique
<
BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,boost::posix_time::ptime, get_timeout)
>,
ordered_unique< secondary_key
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
,composite_key_result_less<secondary_key::result_type>
#endif
>
>
> DataStruct;



struct host_publisher_index{
	std::map<std::string,std::list<std::string>* > hosts;
	bool insert_pair(const std::string& host, const std::string& publisher );
	bool remove_pair(const std::string& host, const std::string& publisher );
	bool host_exists(const std::string& host) const;
	bool has_publisher(const std::string& host, const std::string& pub) const;
	std::list<std::string>* get_hosts() const;
	std::list<std::string>* get_publishers(const std::string& hosts) const;
};

class Filter;

class Blackboard : public Subscriber
{
public:
    Blackboard(unsigned cleanup_period = 300);
    explicit
    Blackboard(const std::string&,unsigned cleanup_period = 300 );
    explicit
    Blackboard(const char*,unsigned cleanup_period = 300 );
//     Blackboard(const Blackboard& other);
    virtual ~Blackboard() {}
    virtual void process_messages();
		template<class Data>
		Data& read_msg(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* tmp =0 ,boost::posix_time::ptime* time_req =0);
		template<class Data>
		Data* read_msg_nb(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* tmp =0 ,boost::posix_time::ptime* time_req =0);
		template<class Data>
		Data* in_msg(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* tmp =0 ,boost::posix_time::ptime* time_req =0);
		template<class Data>
		Data* in_msg_nb(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* tmp =0 ,boost::posix_time::ptime* time_req =0);
		
    template<class Data>
    Data& read(const std::string& type, const std::string&  process,const std::string& host = std::string("localhost"),boost::posix_time::ptime* tmp =0 ,boost::posix_time::ptime* time_req =0);
    template<class Data>
    Data* in(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,bool delete_past = false,boost::posix_time::ptime* tmp =0, boost::posix_time::ptime* time_req = 0);
    template<class Data>
    Data* read_nb(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,boost::posix_time::ptime* tmp =0,boost::posix_time::ptime* time_req=0);
    template<class Data>
    Data* in_nb(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,bool delete_past = false,boost::posix_time::ptime* tmp =0,boost::posix_time::ptime* time_req=0);
    template<class Data>
    std::vector<Data*>* query_in(std::vector<Filter*>& filters);
    template<class Data>
    std::vector<Data*>* query_read(std::vector<Filter*>& filters);
    int add_tuple(Tuple*);
    unsigned get_cleanup_period();
    void set_cleanup_period(unsigned cleanup_period);
		
    template<class Data>
    Data* extract_result_from_tuple(Tuple& t);

		std::list<std::string>* get_hosts() const;
		std::list<std::string>* get_publishers(const std::string& host) const;
private:
		host_publisher_index hosts;
    boost::posix_time::ptime cur_tmsp;
    boost::posix_time::time_duration period;
    DataStruct world_perception;
    int cleanup();
    DataStruct::iterator get_tuple(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req);
    DataStruct::iterator get_tuple(const std::string& type, const std::string&  process,const std::string& host);
		 DataStruct::nth_index<2>::type::iterator get_msg(const std::string& type, const std::string& host ,boost::posix_time::ptime* time_req);
    DataStruct::nth_index<2>::type::iterator get_msg(const std::string& type,const std::string& host);

};


template<class Data>
Data* Blackboard::in_msg(const std::string& type, const std::string& host, boost::posix_time::ptime* tmsp, boost::posix_time::ptime* time_req)
{
    Data* result =0;
		DataStruct::nth_index<2>::type& host_index =  world_perception.get<2>();
		DataStruct::nth_index<2>::type::iterator data_it ;

    if (time_req == 0)
    {
        data_it = get_msg(host,type);
        while (data_it == host_index.end())
        {
            boost::thread::yield();
            process_messages();
            data_it = get_msg(type,host);
            // 			cout << "RUNS" << endl;
        }
    }
    else
    {
        data_it = get_msg(type,host,time_req);
        while (data_it == host_index.end())
        {
            boost::thread::yield();
            process_messages();
            data_it = get_msg(host,type,time_req);
        }

    }
    result = extract_result_from_tuple<Data>(*(*data_it));
    if (tmsp !=0)
        *tmsp = (*data_it)->get_timestamp();
    Tuple *tmp_tuple = (*data_it);
    host_index.erase(data_it);
    tmp_tuple->msg_data = 0;
    delete tmp_tuple;
    return result;
}

template<class Data>
Data* Blackboard::in_msg_nb(const std::string& type, const std::string& host, boost::posix_time::ptime* tmsp, boost::posix_time::ptime* time_req)
{
    Data* result =0;
    		DataStruct::nth_index<2>::type& host_index =  world_perception.get<2>();
		DataStruct::nth_index<2>::type::iterator data_it;

    if (time_req == 0)
    {
        data_it = get_msg(type,host);
    }
    else
    {
        data_it = get_msg(type,host,time_req);
    }


    if (data_it != host_index.end())
    {
        result = extract_result_from_tuple<Data>(*(*data_it));
        if (tmsp !=0)
            *tmsp = (*data_it)->get_timestamp();

        Tuple *tmp_tuple = (*data_it);
        host_index.erase(data_it);
        tmp_tuple->msg_data = 0;
        delete tmp_tuple;
        return result;
    }
    return 0;
}

template<class Data>
Data& Blackboard::read_msg(const std::string& type, const std::string& host, boost::posix_time::ptime* tmsp, boost::posix_time::ptime* time_req)
{
    Data* result =0;
    		DataStruct::nth_index<2>::type& host_index =  world_perception.get<2>();
		DataStruct::nth_index<2>::type::iterator data_it;

    if (time_req == 0)
    {

        data_it = get_msg(type,host);
        while (data_it == host_index.end())
        {
            boost::thread::yield();
            process_messages();
            data_it = get_msg(type,host);
        }
    }
    else
    {
        data_it = get_msg(type,host,time_req);
        while (data_it == host_index.end())
        {
            boost::thread::yield();
            process_messages();
            data_it = get_msg(type,host,time_req);
        }
    }
    result = extract_result_from_tuple<Data>(*(*data_it));
    if (tmsp !=0)
        (*tmsp) = (*data_it)->get_timestamp();
    return (*result);
}

template<class Data>
Data* Blackboard::read_msg_nb(const std::string& type, const std::string& host, boost::posix_time::ptime* tmsp, boost::posix_time::ptime* time_req)
{
    Data* result =0;
    
		DataStruct::nth_index<2>::type& host_index =  world_perception.get<2>();
		DataStruct::nth_index<2>::type::iterator data_it ;
		if (time_req == 0)
    {
        data_it = get_msg(type,host);
    }
    else
    {
        data_it = get_msg(type,host,time_req);
    }
    if (data_it != host_index.end())
    {
        result = extract_result_from_tuple<Data>(*(*data_it));
        if (tmsp !=0)
            *tmsp = (*data_it)->get_timestamp();
        return new Data(*result);
    }
    else
        return 0;
}

template<class Data>
Data* Blackboard::extract_result_from_tuple( Tuple& t)
{
    Data* new_data = 0;
    if (t.meta_data.serialized())
    {
        new_data = new Data();
        RawBytes* serialized_msg = (RawBytes*)t.get_msg_data();
        new_data->ParsePartialFromString(serialized_msg->byte_stream());
        google::protobuf::Message* base_class = dynamic_cast<google::protobuf::Message*>(new_data);
        if (base_class != 0)
        {
            t.msg_data = new_data;
            t.meta_data.set_serialized(false);
            delete serialized_msg;
        }
    }
    else
    {
        new_data = dynamic_cast<Data*>(t.get_msg_data());
    }
    return new_data;
}

template<class Data>
Data& Blackboard::read(const std::string& type, const std::string&  process,const std::string& host,boost::posix_time::ptime* tmsp,boost::posix_time::ptime* time_req)
{
    Data* result =0;
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
    result = extract_result_from_tuple<Data>(*(*data_it));
    if (tmsp !=0)
        (*tmsp) = (*data_it)->get_timestamp();
    return (*result);
}

template<class Data>
Data* Blackboard::in(const std::string& type, const std::string&  process,const std::string& host ,bool delete_past,boost::posix_time::ptime* tmsp ,boost::posix_time::ptime* time_req)
{
    Data* result =0;
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
    result = extract_result_from_tuple<Data>(*(*data_it));
    if (tmsp !=0)
        *tmsp = (*data_it)->get_timestamp();
    Tuple *tmp_tuple = (*data_it);
    world_perception.erase(data_it);
    tmp_tuple->msg_data = 0;
    delete tmp_tuple;
    return result;

}

template<class Data>
Data* Blackboard::read_nb(const std::string& type, const std::string&  process,const std::string& host,boost::posix_time::ptime* tmsp ,boost::posix_time::ptime* time_req)
{
    Data* result =0;
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
        result = extract_result_from_tuple<Data>(*(*data_it));
        if (tmsp !=0)
            *tmsp = (*data_it)->get_timestamp();
        return new Data(*result);
    }
    else
        return 0;
}


template<class Data>
Data* Blackboard::in_nb(const std::string& type, const std::string&  process,const std::string& host ,bool delete_past,boost::posix_time::ptime* tmsp ,boost::posix_time::ptime* time_req)
{
    Data* result =0;
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
        result = extract_result_from_tuple<Data>(*(*data_it));
        if (tmsp !=0)
            *tmsp = (*data_it)->get_timestamp();

        Tuple *tmp_tuple = (*data_it);
        world_perception.erase(data_it);
        tmp_tuple->msg_data = 0;
        delete tmp_tuple;
        return result;
    }
    return 0;
}

template<class Data>
std::vector< Data* >* Blackboard::query_in(std::vector< Filter* >& filters)
{
    //   std::cout << "Unimplemented function query_in" << endl;

    DataStruct::nth_index<1>::type& time_out_index =  world_perception.get<1>();
    DataStruct::nth_index<1>::type::iterator it = time_out_index.begin();
    std::vector<Data* >* result = new std::vector<Data*>();
    while (it != time_out_index.end())
    {
        int i = 0;
        for (i = 0 ; i < filters.size(); i++)
        {
            if (!filters[0]->filter(*(*it)))
                break;
        }
        if (i == filters.size())
        {
            Data *tmp = dynamic_cast<Data*>(extract_result_from_tuple<Data>(*(*it)));
            Tuple *t = (*it);

            time_out_index.erase(it);
						t->msg_data = 0;
            delete t;
						if (tmp != 0)
                result->push_back(tmp);
            continue;
        }
        it++;
    }
    return result;
}

template<class Data>
std::vector< Data* >* Blackboard::query_read(std::vector< Filter* >& filters)
{

    DataStruct::nth_index<1>::type& time_out_index =  world_perception.get<1>();
    DataStruct::nth_index<1>::type::iterator it = time_out_index.begin();
    std::vector<Data* >* result = new std::vector<Data*>();
    while (it != time_out_index.end())
    {

        int i = 0;
        for (i = 0 ; i < filters.size(); i++)
        {
            if (!filters[0]->filter(*(*it)))
                break;
        }
        if (i == filters.size())
        {
            Data *tmp = dynamic_cast<Data*>(extract_result_from_tuple<Data>(*(*it)));
            // time_out_index.erase(it);
            if (tmp != 0)
                result->push_back(new Data(*tmp));
        }
        it++;
    }
    return result;
}
#endif // BLACKBOARD_H
