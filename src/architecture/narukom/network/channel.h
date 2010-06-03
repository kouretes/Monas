
class ChannelCommand;/*
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

#ifndef CHANNEL_H
#define CHANNEL_H
#include<system/thread.h>
#include<pub_sub/publisher.h>
#include<pub_sub/subscriber.h>
#include "host.h"
#include <string>
#include <vector>

class Channel : public Thread, public Publisher, public Subscriber
{
  public:
    explicit
    Channel(const std::string& channel_name);
    std::string get_name() const;
    virtual void start();
    virtual void stop();
    const std::vector<Host*>* get_hosts() const;
    virtual void process_command(const ChannelCommand& cmd);
  private:
    std::string name;
    std::vector<Host*> hosts;
   
  
};

#endif // CHANNEL_H
