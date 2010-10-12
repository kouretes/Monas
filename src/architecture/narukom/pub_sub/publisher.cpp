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
    pub_msg_buf = 0;
    pub_msg_queue = 0;
}

Publisher::Publisher( std::string pub_name )
{
    publisher_name = pub_name;
    pub_msg_buf = 0;
    pub_msg_queue = 0;
}

Publisher::Publisher(const char* pub_name)
{
    publisher_name = string (pub_name);
    pub_msg_buf = 0;
    pub_msg_queue = 0;
}

Publisher::~Publisher()
{
  cout << "Deleting publisher " << endl;
    if (pub_msg_buf != 0)
        pub_msg_queue->remove_publisher(this);
    if (pub_msg_buf != 0)
        delete pub_msg_buf;
}
string Publisher::getName() const
{
  return publisher_name;
}

bool Publisher::operator==(const Publisher& pub_1)
{
  return this->getName() == pub_1.getName();
}
void Publisher::setBuffer(MessageBuffer* buf)
{
  pub_msg_buf = buf;
}

void Publisher::setQueue(MessageQueue* val)
{
  pub_msg_queue = val;
}


void Publisher::publish(const msgentry & msg)
{

    if (pub_msg_buf == 0)
    {
        if (pub_msg_queue == 0)
        {
            cout <<  "publisher: " << getName() << " with neither buffer nor queue " << endl;
            return;
        }
        else
        {
            pub_msg_queue->add_publisher(this);
        }
    }
      pub_msg_buf->add(msg);
}


void Publisher::publish(std::vector<msgentry> vec)
{
  if (pub_msg_buf == 0)
  {
    if (pub_msg_queue == 0)
    {
      cout <<  "publisher: " << getName() << " with neither buffer nor queue " << endl;
      return;
    }
    else
    {
      pub_msg_queue->add_publisher(this);
    }
  }
  pub_msg_buf->add(vec);
}
