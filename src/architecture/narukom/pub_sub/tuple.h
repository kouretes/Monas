#ifndef TUPLE_H
#define TUPLE_H
#include <ostream>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../narukom_messages/Envelope.pb.h"
#include <google/protobuf/message.h>

#include <string>
/**
Tuple class is the container for exchaning data between publishers and subscribers

*/
using namespace boost::multi_index;
using namespace boost::posix_time;
class Blackboard;
class Tuple{
  friend class Blackboard;
  public:
explicit
Tuple(google::protobuf::Message* msg,const std::string& host = "localhost",const std::string pub_name = "", const std::string topic = "global", const std::string& destination = "",int timeout = 100);
Tuple(google::protobuf::Message* msg,const Envelope& metadata);
Tuple(char* bytes,unsigned int size,
      const std::string& host = "localhost",const std::string pub_name = "", const std::string topic = "global", const std::string& destination = "",int timeout = 100);
Tuple(char* bytes,unsigned size, const Envelope& metadata);      
Tuple(const Tuple& other);
  
  const std::string& get_host() const;// {return meta_data.host();}
  const std::string& get_publisher() const;// {return meta_data.publisher();}
  std::string get_type() const;// {return meta_data.type();}
  const std::string& get_topic() const;// {return meta_data.topic();}
  boost::posix_time::ptime get_timestamp() const;// {return boost::posix_time::from_iso_string(meta_data.timestamp());}
  boost::posix_time::ptime get_timeout() const;// {return timeout;}
  google::protobuf::Message* get_msg_data() const;// {return msg_data; }
  const Envelope& get_envelope() const;//{return meta_data;}
  friend std::ostream&  operator<<(std::ostream& os, const Tuple& t);

  ///This function generates the timeout of the tuple if the parameter from_now is true then the timestamp of the tuple is being updated
  ///and then the timeout is generated otherwise the timeout will be generated from the current timestamp of the tuple! 
  ~Tuple();
  void generate_timeout(bool from_now);
   
  private:
    Envelope meta_data;
    google::protobuf::Message* msg_data;
    boost::posix_time::ptime timeout;
};


struct standard_key : composite_key<
	Tuple,
	BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,const std::string&, get_host),
	BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,const std::string&, get_publisher),
	BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,std::string, get_type),
	BOOST_MULTI_INDEX_CONST_MEM_FUN(Tuple,boost::posix_time::ptime, get_timestamp)
>{};



#endif
