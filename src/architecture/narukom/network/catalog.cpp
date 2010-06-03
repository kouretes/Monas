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

#include "catalog.h"

using std::vector;
using std::map;
using std::string;
Catalog::Catalog()
{
  readConfiguration("channels.xml");
}

Catalog::Catalog(const std::string& configuration_file): Publisher(configuration_file)
{
  readConfiguration(configuration_file);
}

std::vector< Channel*, std::allocator< Channel* > >& Catalog::get_list_of_channels() const
{
  std::vector<Channel*>* result = new vector<Channel*>();
  std::map<std::string,Channel*>::const_iterator it = channels.begin();
  while(it != channels.end())
  {
    result->push_back(it->second);
    it++;
  }
  return *result;
}


std::vector< Host* >& Catalog::get_list_of_hosts() const
{
  std::vector<Host*>* result = new vector<Host*>();
  map<string,Host*>::const_iterator it = hosts.begin();
  while(it != hosts.end())
  {
    result->push_back(it->second);
    it++;
  }
  return *result;
}

void Catalog::readConfiguration(const std::string& filename)
{
  
}


bool Catalog::register_channel(Channel* ch)
{
  channels.insert(std::make_pair<string,Channel*>(ch->get_name(),ch));
  //TODO  get_hosts
  return true;
}

void Catalog::send_channel_command(const string channel,const ChannelCommand& cmd)
{
  map<string,Channel*>::iterator it = channels.find(channel);
  if(it != channels.end())
  {
    it->second->process_command(cmd);
  }
}
bool Catalog::start_channel(const std::string& name)
{
  map<string,Channel*>::iterator it = channels.find(name);
  if(it != channels.end())
  {
     it->second->start();
     return true;
  }
  return false;
}



bool Catalog::stop_channel(const std::string& name)
{
  map<string,Channel*>::iterator it = channels.find(name);
  if(it != channels.end())
  {
    it->second->stop();
    return true;
  }
  return false;
}

bool Catalog::unregister_channel(const std::string& name)
{
  //TODO remove from hosts
  map<string,Channel*>::iterator it = channels.find(name);
  if(it != channels.end())
  {
    channels.erase(it);
    return true;
  }
  return false;
}
