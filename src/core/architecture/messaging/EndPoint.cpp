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
#include "EndPoint.hpp"
#include "core/architecture/time/SystemTime.hpp"
#include "MessageBuffer.hpp"
#include "MessageEntry.hpp"
#include "TopicTree.hpp"


namespace Messaging
{

    using namespace std;
    using namespace KSystem;

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


    void EndPoint::updateSubscription(std::string const& topic , MessageEntry::msgclass_t where, std::size_t host)
    {
        //cout<<"Check -2"<<endl;
        if(read_buf == NULL || write_buf == NULL)
            return;

        //cout<<"Check -1"<<endl;
        MessageEntry nmsg;
        nmsg.topic = Topics::Instance().getId(topic);
        nmsg.host = host;
        nmsg.msgclass = where;
        publish(nmsg);
    }


    void EndPoint::publish(const MessageEntry & msg)
    {
        if (write_buf == 0)
            return;

        write_buf->add(msg);
    }


    void EndPoint::publish(std::vector<MessageEntry> const& vec)
    {
        if (write_buf == 0)
            return ;

        write_buf->add(vec);
    }



    void EndPoint::publishData(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        //cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        Time::TimeStamp now = Time::SystemTime::now().wrapTo<Time::TimeStamp>();
        //nmsg.timeoutstamp=now+KSystem::Time::millisec(timeout);
        nmsg.timestamp = now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        nmsg.msgclass = MessageEntry::DATA;
        publish(nmsg);
    }

    void EndPoint::publishSignal(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        //cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        Time::TimeStamp now = Time::SystemTime::now().wrapTo<Time::TimeStamp>();
        //nmsg.timeoutstamp=now+KSystem::Time::millisec(timeout);
        nmsg.timestamp = now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        nmsg.msgclass = MessageEntry::SIGNAL;
        publish(nmsg);
    }

    void EndPoint::publishState(const google::protobuf::Message & msg, std::string const& topic)
    {
        MessageEntry nmsg;
        google::protobuf::Message * newptr = msg.New();
        newptr->CopyFrom(msg);
        nmsg.msg.reset(newptr);
        //cout<<"In:"<<&msg;
        //cout<<"Copy:"<<nmsg.msg<<endl;
        nmsg.host = MessageEntry::HOST_ID_LOCAL_HOST;
        Time::TimeStamp now = Time::SystemTime::now().wrapTo<Time::TimeStamp>();
        //nmsg.timeoutstamp=now+KSystem::Time::millisec(timeout);
        nmsg.timestamp = now;
        nmsg.topic = Topics::Instance().getId(topic);
        //nmsg.publisher=Publisher::getName();
        nmsg.msgclass = MessageEntry::STATE;
        publish(nmsg);
    }

    std::vector<MessageEntry> EndPoint::remove()
    {
        std::vector<MessageEntry> data;

        if(read_buf != 0)
            data = read_buf->remove();

        return data;
    }
}

