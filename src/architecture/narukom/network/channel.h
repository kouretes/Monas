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

#ifndef CHANNEL_H
#define CHANNEL_H
#include <string>
#include <vector>
#include "publisher.h"
#include "subscriber.h"
#include "Thread.h"
#include <google/protobuf/message.h>
class ChannelCommand;

class Channel : public Thread, public Publisher, public Subscriber
{
public:
	std::vector<std::string>* available_peers();
	std::vector<std::string>* available_messages();
	bool process_command(const ChannelCommand& cmd);
	void publish(google::protobuf::Message* msg);
	void process_messages();
	void run();
private:
	std::vector<std::string>* messages;
	std::vector<std::string>* reachable_peers;
};

#endif // CHANNEL_H
