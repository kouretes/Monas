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

#ifndef MSG_H
#define MSG_H

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <google/protobuf/message.h>

typedef struct msgentry_{
        enum msgclass_t{ DATA, SIGNAL,STATE};
        boost::shared_ptr<const google::protobuf::Message > msg;
        boost::posix_time::ptime timestamp;
        boost::posix_time::ptime timeoutstamp;
        std::string topic;
        std::string host;
        std::string publisher;
        msgclass_t msgclass;
        bool operator== (const struct msgentry_ & b){ return msg==b.msg;};
    } msgentry;

struct msgentrytimestampComparator
{

   bool operator()(const msgentry &a,const msgentry &b) {return a.timestamp < b.timestamp;};
};





#endif /* MSG_H */
