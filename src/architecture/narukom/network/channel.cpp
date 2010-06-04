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

#include "channel.h"

Channel::Channel(const std::string& channel_name) : Thread(false), Publisher(channel_name),Subscriber(channel_name) , name(channel_name)
{
}
std::string Channel::get_name() const
{
  return name;
}

const std::vector< Host*>* Channel::get_hosts() const
{
  return &hosts;
}

void Channel::process_command(const ChannelCommand& cmd)
{
  //TODO
  //switch(type)
  //{ execute command
  //
  //}
}

void Channel::start()
{
  Thread::StartThread();
}

void Channel::stop()
{
  Thread::StopThread();
}

