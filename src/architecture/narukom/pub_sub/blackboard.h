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

#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "subscriber.h"

#include "Mutex.h"

#include <string>

//#include <auto_ptr.h>
#include "Basic.pb.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/asio.hpp>
#include <boost/concept_check.hpp>
#include "tuple.h"
class Blackboard;

class Agent : public Thread, public Publisher{
  public:

    Agent();
    Agent(std::string name) ;
    Agent(const char* name );
    std::string get_name() const {return agent_name;}
    virtual void run();
    virtual void publish(google::protobuf::Message* msg){
      Publisher::publish(msg);
    }
    Blackboard* getBlackboard() const {return blk;}
  private:
    std::string agent_name;
    Blackboard* blk;
};




typedef std::pair<boost::posix_time::ptime, google::protobuf::Message*> message_pair;
typedef std::vector<message_pair> msg_type;
typedef std::map<std::string,msg_type* > msg_col;
typedef std::map<std::string, msg_col* > robot;
typedef std::map<std::string, robot* > perception ;

using namespace boost::multi_index;

typedef multi_index_container<
	Tuple*,
	indexed_by<
		ordered_unique< standard_key
		#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
      ,composite_key_result_less<standard_key::result_type>
#endif
		>,
		ordered_non_unique
		<
		BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,boost::posix_time::ptime, get_timeout)
		>
	>
> DataStruct;
class Blackboard : public Subscriber
{
  public:
		
    Blackboard();
    Blackboard(const std::string& );
    Blackboard(const char* );
    Blackboard(const Subscriber& other);
    Blackboard(const Blackboard& other);
    Blackboard(Agent* agent );
    virtual ~Blackboard(){}
    virtual void process_messages();
		
    google::protobuf::Message* read(const std::string& type, const std::string&  process,const std::string& host = std::string("localhost") ,boost::posix_time::ptime* time_req =0);
    google::protobuf::Message* in(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,boost::posix_time::ptime* time_req = 0);
    google::protobuf::Message* read_nb(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,boost::posix_time::ptime* time_req=0);
    google::protobuf::Message* in_nb(const std::string& type, const std::string&  process,const std::string& host = "localhost" ,boost::posix_time::ptime* time_req=0);
    Agent* getAgent() const {return myagent;}
    int add_tuple(Tuple*);

  private:

    boost::posix_time::ptime cur_tmsp;

		boost::posix_time::time_duration period;
    Agent*  myagent;
    DataStruct world_perception;
    Mutex data_struct_mx;
    int cleanup();
    DataStruct::iterator get_tuple(const std::string& type, const std::string&  process,const std::string& host ,boost::posix_time::ptime* time_req);
    DataStruct::iterator get_tuple(const std::string& type, const std::string&  process,const std::string& host);

};

#endif // BLACKBOARD_H
