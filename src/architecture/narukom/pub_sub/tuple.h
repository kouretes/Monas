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
#include "Basic.pb.h"
using namespace boost::multi_index;
using namespace boost::posix_time;
struct Tuple{
Tuple(google::protobuf::Message* msg): msg(msg)
{ 
	google::protobuf::int32 tm = ((BasicMessage*)msg)->timeout();
	if( tm < 0 )
		timeout = boost::posix_time::from_iso_string("9999-Dec-31");
	else
		timeout = boost::posix_time::from_iso_string(((BasicMessage*)msg)->timestamp()) + boost::posix_time::millisec(tm);

}
  const std::string& get_host() const {return ((BasicMessage*)msg)->host();}
  const std::string& get_publisher() const {return ((BasicMessage*)msg)->publisher();}
  std::string get_type() const {return ((BasicMessage*)msg)->GetTypeName();}
  boost::posix_time::ptime get_timestamp() const {return boost::posix_time::from_iso_string(((BasicMessage*)msg)->timestamp());}
  boost::posix_time::ptime get_timeout() const {return timeout;}
  google::protobuf::Message* msg;
  friend std::ostream&  operator<<(std::ostream& os, const Tuple& t)
  {
		os << "host: " << t.get_host() << " publisher " << t.get_publisher() << " type " << t.get_type() << " tm: " << boost::posix_time::to_iso_string(t.get_timestamp());
		return os;
  }
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