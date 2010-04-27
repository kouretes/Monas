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
//#include "../pingpong.pb.h"
#include "publisher.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using std::string;
using google::protobuf::Message;
#include <google/protobuf/descriptor.h>

Publisher::Publisher()  
{
  publisher_name = string("Default Publisher");
  pub_msg_buf = NULL;
  pub_msg_queue = NULL;
}
  
Publisher::Publisher( std::string pub_name )
{
  publisher_name = pub_name;
  pub_msg_buf = NULL;
  pub_msg_queue = NULL;
}

Publisher::Publisher(const char* pub_name) 
{
  publisher_name = string (pub_name);
  pub_msg_buf = NULL;
  pub_msg_queue = NULL;
}
Publisher::Publisher(const Publisher& other) 
{
  MessageBuffer* tmp;
  publisher_name = other.getName();
  pub_msg_buf->copyFrom(*(other.getBuffer()) );
  pub_msg_queue = other.getQueue();
}
Publisher::~Publisher()
{
  if(pub_msg_buf != NULL)
    pub_msg_queue->remove_publisher(this);
  if(pub_msg_buf != NULL)
    delete pub_msg_buf;
}

void Publisher::publish(Message* msg)
{
  //  std::cout << "Publish << endl" << endl; 
		static string localhost = "localhost";
    if(pub_msg_buf == NULL)
    {  if(pub_msg_queue == NULL)
      {
				cout <<  "publisher: " << getName() << " with neither buffer nor queue " << endl;
				return;
      }
      else
      {
			pub_msg_queue->add_publisher(this);
      }
    }
//     Message* new_msg = msg->New();
//     new_msg->CopyFrom(*msg);
		if(msg != 0)
		{
			const google::protobuf::Descriptor* descriptor = msg->GetDescriptor();
			const google::protobuf::Reflection*    reflection = msg->GetReflection();
			const google::protobuf::FieldDescriptor* string_field =  descriptor->FindFieldByName("host");
 			reflection->SetString(msg,string_field,localhost);
			string_field = descriptor->FindFieldByName("publisher");
			reflection->SetString(msg,string_field,publisher_name);
			string_field = descriptor->FindFieldByName("timestamp");
			reflection->SetString(msg,string_field,boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time() ));
			
			pub_msg_buf->add(msg);
		}
//     cout << "Publsh: " << pub_msg_buf->size() << endl;
}
