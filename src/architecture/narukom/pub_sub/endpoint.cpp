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
#include "endpoint.h"
#include "message_buffer.h"
#include "topicTree.h"

using namespace std;

EndPoint::EndPoint(const std::string& sb_name): endpoint_name(sb_name), write_buf(NULL), read_buf(NULL)
{
}

EndPoint::~EndPoint()
{
	if(read_buf != NULL)
		delete read_buf;

	if(write_buf != NULL)
		delete write_buf;
}


void EndPoint::updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::size_t host)
{
	//cout<<"Check -2"<<endl;
	if(read_buf == NULL || write_buf == NULL)
		return;

	//cout<<"Check -1"<<endl;
	msgentry nmsg;
	nmsg.topic = Topics::Instance().getId(topic);
	nmsg.host = host;
	nmsg.msgclass = where;
	publish(nmsg);
}


void EndPoint::publish(const msgentry & msg)
{
	if (write_buf == 0)
		return;

	write_buf->add(msg);
}


void EndPoint::publish(std::vector<msgentry> const& vec)
{
	if (write_buf == 0)
		return ;

	write_buf->add(vec);
}



void EndPoint::publishData(const google::protobuf::Message & msg, std::string const& topic)
{
	msgentry nmsg;
	google::protobuf::Message * newptr = msg.New();
	newptr->CopyFrom(msg);
	nmsg.msg.reset(newptr);
	//cout<<"In:"<<&msg;
	//cout<<"Copy:"<<nmsg.msg<<endl;
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	//nmsg.timeoutstamp=now+boost::posix_time::millisec(timeout);
	nmsg.timestamp = now;
	nmsg.topic = Topics::Instance().getId(topic);
	//nmsg.publisher=Publisher::getName();
	nmsg.msgclass = msgentry::DATA;
	publish(nmsg);
}

void EndPoint::publishSignal(const google::protobuf::Message & msg, std::string const& topic)
{
	msgentry nmsg;
	google::protobuf::Message * newptr = msg.New();
	newptr->CopyFrom(msg);
	nmsg.msg.reset(newptr);
	//cout<<"In:"<<&msg;
	//cout<<"Copy:"<<nmsg.msg<<endl;
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	//nmsg.timeoutstamp=now+boost::posix_time::millisec(timeout);
	nmsg.timestamp = now;
	nmsg.topic = Topics::Instance().getId(topic);
	//nmsg.publisher=Publisher::getName();
	nmsg.msgclass = msgentry::SIGNAL;
	publish(nmsg);
}

void EndPoint::publishState(const google::protobuf::Message & msg, std::string const& topic)
{
	msgentry nmsg;
	google::protobuf::Message * newptr = msg.New();
	newptr->CopyFrom(msg);
	nmsg.msg.reset(newptr);
	//cout<<"In:"<<&msg;
	//cout<<"Copy:"<<nmsg.msg<<endl;
	nmsg.host = msgentry::HOST_ID_LOCAL_HOST;
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	//nmsg.timeoutstamp=now+boost::posix_time::millisec(timeout);
	nmsg.timestamp = now;
	nmsg.topic = Topics::Instance().getId(topic);
	//nmsg.publisher=Publisher::getName();
	nmsg.msgclass = msgentry::STATE;
	publish(nmsg);
}

std::vector<msgentry> EndPoint::remove()
{
	std::vector<msgentry> data;

	if(read_buf != 0)
		data = read_buf->remove();

	return data;
}

