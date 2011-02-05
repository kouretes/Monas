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

#include "subscriber.h"
#include "message_queue.h"
#include "message_buffer.h"
using namespace std;

Subscriber::Subscriber(): subscriber_name("Default Subscriber"),sub_msg_buf(NULL)
{

}

Subscriber::Subscriber(const string& sb_name):subscriber_name(sb_name),sub_msg_buf(NULL)
{

}

Subscriber::~Subscriber()
{
  cout << "Deleting subscriber " << endl;

  if(sub_msg_buf != 0)
    delete sub_msg_buf;

}


void Subscriber::attachSubscriberToMessageQueue(MessageQueue & q)
{
	sub_msg_buf=q.attachSubscriber(subscriber_name);
}

void Subscriber::subscribeTo(std::string const& topic , int where)
{
	//cout<<"Check -2"<<endl;
	if(sub_msg_buf==NULL)
		return;
	//cout<<"Check -1"<<endl;
	sub_msg_buf->getQueue().subscribeTo(sub_msg_buf,topic,where);
}

void Subscriber::unsubscribeFrom(std::string const& topic , int where)
{
	if(sub_msg_buf==NULL)
		return;
	sub_msg_buf->getQueue().unsubscribeFrom(sub_msg_buf,topic,where);
}


void Subscriber::process_messages(){

	cout << "Subscriber Process_msg  called " << endl;
	remove();
}
std::vector<msgentry> Subscriber::remove()
{
    std::vector<msgentry> data;
    if(sub_msg_buf == 0)
    {
    	cout<<"Subscriber::process_messages() :not attached to a messageQueue"<<endl;

    }
    else
		data=sub_msg_buf->remove();
    return data;
}

