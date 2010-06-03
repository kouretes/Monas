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

#include "host.h"
using std::string;
using std::map;
std::string Host::get_address(std::string channel_name) const
{
  map<string,string>::const_iterator it = addresses.find(channel_name);
  if(it != addresses.end())
    return it->second;
  return "";
}

const std::map< std::string, std::string>& Host::get_adresses() const
{
  return addresses;
}

const std::string& Host::get_name() const
{
  return hostname;
}

Host::Host(const std::string& hostname) : hostname(hostname)
{
  
}


void Host::add_address(string channel_name, string address)
{
  addresses.insert(std::make_pair<string,string>(channel_name,address));
}
