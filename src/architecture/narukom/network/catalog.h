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

#ifndef CATALOG_H
#define CATALOG_H
#include <vector>
#include <string>
#include <map>
#include <pub_sub/publisher.h>
#include <pub_sub/subscriber.h>
#include <system/thread.h>
#include "channel.h"
#include "host.h"
class ChannelCommand;

class Catalog: public Publisher,public Subscriber,public Thread
{
  public: 
    Catalog();
    Catalog(const std::string& configuration_file);
    std::vector<Channel*>& get_list_of_channels() const;
    std::vector<Host*>& get_list_of_hosts() const;
    bool register_channel(Channel * );
    bool unregister_channel(const std::string& name);
    bool start_channel(const std::string& name);
    bool stop_channel(const std::string& name);
    void send_channel_command(const std::string channel,const ChannelCommand& cmd);
  private:
    
    std::map<std::string,Channel*> channels;
    std::map<std::string,Host*>  hosts;
    void readConfiguration(const std::string& filename);
};

#endif // CATALOG_H
