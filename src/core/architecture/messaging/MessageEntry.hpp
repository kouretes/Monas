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

#ifndef MESSAGEENTRY_HPP
#define MESSAGEENTRY_HPP

#include <boost/shared_ptr.hpp>
#include <core/architecture/time/TimeTypes.hpp>
#include <google/protobuf/message.h>

namespace Messaging
{
    class MessageEntry
    {
        public:
        enum msgclass_t { DATA, SIGNAL, STATE,
                          SUBSCRIBE_ON_TOPIC,
                          SUBSCRIBE_ABOVE_TOPIC,
                          SUBSCRIBE_BELOW_TOPIC,
                          SUBSCRIBE_ALL_TOPIC,
                          UNSUBSCRIBE_ON_TOPIC,
                          UNSUBSCRIBE_ABOVE_TOPIC,
                          UNSUBSCRIBE_BELOW_TOPIC,
                          UNSUBSCRIBE_ALL_TOPIC
                        };
        static const std::size_t HOST_ID_ANY_HOST = 0;
        static const std::size_t HOST_ID_LOCAL_HOST = 1;
        boost::shared_ptr<const google::protobuf::Message > msg;
        KSystem::Time::TimeStamp timestamp;
        //KSystem::Time::TimeAbsolute timeoutstamp;
        std::size_t topic;
        std::size_t host;
        //std::string publisher;
        msgclass_t msgclass;
        bool operator== (const struct MessageEntry & b) const
        {
            return msg == b.msg;
        };

        bool operator<(const MessageEntry & b) const
        {
            return timestamp< b.timestamp;
        };

        bool operator<(const KSystem::Time::TimeStamp &b) const
        {
            return timestamp< b;
        };

    };



}

#endif /* MESSAGEENTRY_HPP */
