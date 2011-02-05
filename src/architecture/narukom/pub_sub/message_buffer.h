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

#ifndef MESSAGE_BUFFER_H
#define MESSAGE_BUFFER_H
#include <boost/thread/condition_variable.hpp>
#include <vector>
#include <google/protobuf/message.h>
#include "msg.h"
#include "filters/filter.h"
#include <queue>
#include <list>

class MessageQueue;
class MessageBuffer
{
  public:

    explicit
    MessageBuffer(std::size_t nid,MessageQueue &amq , bool notifier);
    ~MessageBuffer();
    void add( std::vector<msgentry> const & tuples);
    bool tryadd( std::vector<msgentry> const & tuples);

    void add(const msgentry & t);
    std::vector<msgentry> remove();
    //bool operator==( MessageBuffer& other) ;
    std::size_t getOwnerID() const {return ownerId;};
    MessageQueue & getQueue() {return  mq;};
    void add_filter(Filter* filter);
    void remove_filter(Filter* filter);
  private:

    std::vector<msgentry> msg_buf;
    std::list<Filter*> filters;
    std::size_t ownerId;
    boost::mutex  mutex;
    MessageQueue &mq;
    bool shouldNotify;

};

#endif // MESSAGE_BUFFER_H
