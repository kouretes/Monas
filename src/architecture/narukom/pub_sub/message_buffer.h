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
#include "../system/Mutex.h"
#include "tuple.h"
#include "filters/filter.h"
#include <list>

class MessageBuffer
{
  public:
    MessageBuffer(Mutex* mx = 0,boost::condition_variable_any* cv = 0);
    explicit
    MessageBuffer(const std::string owner,Mutex* mx,boost::condition_variable_any* cv );
    MessageBuffer(MessageBuffer&);
    ~MessageBuffer();
    int size() ;//{ return msg_buf->size();}
    bool isEmpty()  ;//{ return !(msg_buf->size() > 0);}
    void clear();//{ msg_buf->clear();}
    void copyFrom( MessageBuffer&);
    void mergeFrom( MessageBuffer&);
    void add(Tuple* msg);
    bool operator==( MessageBuffer& other) ;
    Tuple* remove( std::vector< Tuple* >::iterator );
    Tuple* remove_head();
    Tuple* remove_tail();
    std::list< Tuple* >::iterator get_iterator();
    std::list< Tuple* >::iterator end();
    std::list<Tuple*>&   getBuffer() ;//   {return *msg_buf;}
    std::string getOwner() ;// {return owner;}
    boost::condition_variable_any* get_condition_variable() ;
		Mutex* get_queue_mutex() ;
    void add_filter(Filter* filter);
    void remove_filter(Filter* filter);
  private:
    std::list<Tuple*>* msg_buf;
    std::list<Filter*> filters;
    std::string owner; 
    Mutex mutex;
		Mutex *mq_mutex;
    boost::condition_variable_any* mq_cv;
  
};

#endif // MESSAGE_BUFFER_H
