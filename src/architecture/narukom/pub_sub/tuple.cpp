#include "tuple.h"
Tuple::Tuple(google::protobuf::Message* msg, const Envelope& metadata)
{
  meta_data.CopyFrom(metadata);
  msg_data = msg->New();
  msg_data->CopyFrom(*msg);
}

Tuple::Tuple(google::protobuf::Message* msg, const std::string& host, const std::string pub_name, const std::string topic, const std::string& destination, int timeout)
{
  meta_data.set_topic(topic);
  meta_data.set_destination(destination);
  meta_data.set_publisher(pub_name);
  meta_data.set_timeout(timeout);
  meta_data.set_type(msg->GetTypeName());
  meta_data.set_host(host);
  if(meta_data.type() == "RawBytes")
    meta_data.set_serialized(true);
  else
    meta_data.set_serialized(false);
  msg_data = msg->New();
  msg_data->CopyFrom(*msg);
  
  meta_data.set_timestamp( boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()));
}

Tuple::Tuple(char* bytes, unsigned int size, const std::string& host, const std::string pub_name, const std::string topic, const std::string& destination, int timeout)
{
  RawBytes* serialized_msg = new RawBytes();
  serialized_msg->set_byte_stream(bytes,size);
  serialized_msg->set_size(size);
  meta_data.set_topic(topic);
  meta_data.set_destination(destination);
  meta_data.set_publisher(pub_name);
  meta_data.set_timeout(timeout);
  meta_data.set_type(serialized_msg->GetTypeName());
  meta_data.set_host(host);
  meta_data.set_serialized(true);
  msg_data = serialized_msg;
  meta_data.set_timestamp( boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()));
}
Tuple::Tuple(char* bytes, unsigned int size, const Envelope& metadata)
{
  RawBytes* serialized_msg = new RawBytes();
  serialized_msg->set_byte_stream(bytes,size);
  serialized_msg->set_size(size);
  meta_data.CopyFrom(metadata);
  meta_data.set_serialized(true);
  msg_data = serialized_msg;
}
Tuple::Tuple(const Tuple& other)
{
  meta_data.CopyFrom(other.get_envelope());
  msg_data->CopyFrom(*other.get_msg_data());
  timeout = other.get_timeout();
}
Tuple::~Tuple()
{
  delete msg_data;
  
}
const Envelope& Tuple::get_envelope() const
{
  return meta_data;
}

const std::string& Tuple::get_host() const
{
  return meta_data.host();
}

google::protobuf::Message* Tuple::get_msg_data() const
{
  return msg_data;
}
const std::string& Tuple::get_publisher() const
{
  return meta_data.publisher();
}

boost::posix_time::ptime Tuple::get_timeout() const
{
  return timeout;
}

boost::posix_time::ptime Tuple::get_timestamp() const
{
  return boost::posix_time::from_iso_string(meta_data.timestamp());
}

const std::string& Tuple::get_topic() const
{
  return meta_data.topic();
}

std::string Tuple::get_type() const
{
  return meta_data.type();
}
std::ostream& operator<<(std::ostream& os, const Tuple& t)
{
  os << "host: " << t.get_host() << " publisher " << t.get_publisher() << " type " << t.get_type() << " tm: " << boost::posix_time::to_iso_string(t.get_timestamp());
  return os;
}





void Tuple::generate_timeout(bool from_now)
{
  
  if(from_now)
  {
    meta_data.set_timestamp( boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()));
    timeout = boost::posix_time::from_iso_string(meta_data.timestamp()) + boost::posix_time::millisec(meta_data.timeout());
  
  }
  else
  {
    timeout = boost::posix_time::from_iso_string(meta_data.timestamp()) + boost::posix_time::millisec(meta_data.timeout());
  }
      
}
