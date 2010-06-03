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

#ifndef CONTENT_FILTER_H
#define CONTENT_FILTER_H

#include "filter.h"
#include <google/protobuf/descriptor.h>
using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::Reflection;
using google::protobuf::FieldDescriptor;
using google::protobuf::int32;
using google::protobuf::uint32;
using google::protobuf::uint64;
using google::protobuf::int64;

template<class Value>
class ContentFilter : public Filter
{
 
  
  public:
    explicit
    ContentFilter(const std::string& type_name,bool value =false): Filter(type_name,value){
    tupleAttribute = "";
    }
    explicit
    ContentFilter(const std::string& type_name, const std::string& attr_name, const Value& val, bool cvalue = false) : Filter(type_name,cvalue)
    {  tupleAttribute = attr_name;
      value = val;
    }
    virtual FilterState filter(const Tuple& a_tuple)
    {
      
      if(a_tuple.get_envelope().serialized())
      {
	return Unknown;
      }
      else
      {
	google::protobuf::Message* tmp_msg = a_tuple.get_msg_data();
	const google::protobuf::Descriptor* descriptor = tmp_msg->GetDescriptor();
	const google::protobuf::Reflection* reflection = tmp_msg->GetReflection();
	const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName(tupleAttribute);
	FilterState state = check_equal(tmp_msg,reflection,field,value);
	if(!Filter::get_complement())
	  return state;
	else
	  if(state == Accepted)
	    return Rejected;
	   else
	     return Accepted;
	  
	  
	
	return Unknown;
      }
      
    }
    void setTupleAttribute(const std::string& attr_name){ tupleAttribute = attr_name;}
    void setValue(const Value& val){value = val;}

  private:
    std::string tupleAttribute;
    Value value;
    //bool double enum float int32 int64 uint32 uint64 string
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,std::string val)
    {
      if(value == reflection->GetString(*msg,field))
	return Accepted;
      return Rejected;
    }
    
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,int32 val)
    {
      if(value == reflection->GetInt32(*msg,field))
	return Accepted;
      return Rejected;
    }
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,uint32 val)
    {
      if(value == reflection->GetUInt32(*msg,field))
	return Accepted;
      return Rejected;
    }
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,int64 val)
    {
      if(value == reflection->GetInt64(*msg,field))
	return Accepted;
      return Rejected;
    }
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,uint64 val)
    {
      if(value == reflection->GetUInt64(*msg,field))
	return Accepted;
      return Rejected;
    }
    
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,float val)
    {
      if(value == reflection->GetFloat(*msg,field))
	return Accepted;
      return Rejected;
    }
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,double val)
    {
      if(value == reflection->GetDouble(*msg,field))
	return Accepted;
      return Rejected;
    }
    FilterState check_equal(const Message* msg, const Reflection* reflection, const FieldDescriptor* field,bool val)
    {
      if(value == reflection->GetBool(*msg,field))
	return Accepted;
      return Rejected;
    }
    
};

#endif // CONTENT_FILTER_H
