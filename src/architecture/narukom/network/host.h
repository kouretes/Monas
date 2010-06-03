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

#ifndef HOST_H
#define HOST_H
#include <string>
#include <vector>
#include <map>
class Host
{
  public:
    Host(const std::string& hostname);
    const std::string& get_name() const;
    std::string get_address(std::string channel_name) const;
    const std::map<std::string,std::string>& get_adresses() const;
    void add_address(std::string channel_name,std::string address);
  private:
    std::string hostname;
    std::vector<std::string> messages;
    std::map<std::string,std::string> addresses;
    
};

#endif // HOST_H
