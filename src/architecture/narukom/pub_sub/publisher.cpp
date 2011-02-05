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
#include "message_queue.h"
#include "message_buffer.h"

using std::string;
using namespace std;


Publisher::Publisher() :publisher_name("Default Publisher"),pub_msg_buf(NULL)
{

}

Publisher::Publisher( std::string pub_name ):publisher_name(pub_name),pub_msg_buf(NULL)
{

}


Publisher::~Publisher()
{
  cout << "Deleting publisher " << endl;
    if (pub_msg_buf != NULL)
        delete pub_msg_buf;
}



void Publisher::attachPublisherToMessageQueue(MessageQueue & q)
{
	pub_msg_buf=q.attachPublisher(publisher_name);
}




void Publisher::publish(const msgentry & msg)
{

    if (pub_msg_buf == 0)
    {
    	 cout <<  "Publisher::publish() : not attached to a messageQueue" << endl;
    	 return;
    }
	pub_msg_buf->add(msg);
}


void Publisher::publish(std::vector<msgentry> const& vec)
{
	if (pub_msg_buf == 0)
	{
		cout <<  "Publisher::publish() : not attached to a messageQueue" << endl;
		return;
	}
	pub_msg_buf->add(vec);
}
