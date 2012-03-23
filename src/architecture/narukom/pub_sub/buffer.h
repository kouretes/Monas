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

#ifndef BUFFER_H
#define BUFFER_H
#include "hal/mutex.h"
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <vector>
#include <list>


template<typename T>
class Buffer
{
  public:
  	typedef boost::function1<void,Buffer<T> *>  NotifierFncPtr;

    explicit
    Buffer(std::size_t nid);
    ~Buffer();
    void add( std::vector<T> const & tuples);
    bool tryadd( std::vector<T> const & tuples);

    void add(const T & t);
    std::vector<T> remove();
    std::vector<T> tryremove();
    //bool operator==( MessageBuffer& other) ;
    std::size_t getOwnerID() const {return ownerId;};
    //MessageQueue & getQueue() {return  mq;};
    void setNotifier(NotifierFncPtr an) {KSystem::Mutex::scoped_lock data_lock(mutex);n=an;};
    void setCleanUp(NotifierFncPtr ac) {KSystem::Mutex::scoped_lock ata_lock(mutex);c=ac;};
  private:

    std::vector<T> msg_buf;
    std::size_t ownerId;
    KSystem::Mutex  mutex;
    NotifierFncPtr n;
    NotifierFncPtr c;
};

#endif // BUFFER_H
