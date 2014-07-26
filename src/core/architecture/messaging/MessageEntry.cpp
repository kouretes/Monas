/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "MessageEntry.hpp"
#include <google/protobuf/descriptor.h>
#include <iostream>
namespace Messaging
{
    google::protobuf::Message * getFromFactory(std::string const&str)
    {

		const google::protobuf::Descriptor *d = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(str);

		if(!d)
		{
			std::cout << "Could Not Find Descriptor for:" << str << std::endl;
			return NULL;
		}

		google::protobuf::Message *protomsg = google::protobuf::MessageFactory::generated_factory()->GetPrototype(d)->New();
		return protomsg;
	}


}

