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

#ifndef ENDPOINT_H
#define ENDPOINT_H
#include <string>
#include <vector>
#include "msg.h"
#include "message_queue.h"
//MessageBuffer forward Decl
template<typename T>class Buffer;
typedef  Buffer<msgentry> MessageBuffer;
class EndPoint
{
public:
	EndPoint(std::string const& name);
	virtual ~EndPoint();

	void publish(  msgentry const& msg);
	void publish(std::vector<msgentry> const& vec);

	virtual void publishData(const google::protobuf::Message & msg, std::string const& topic);
	virtual void publishSignal(const google::protobuf::Message & msg, std::string const& topic);
	virtual void publishState(const google::protobuf::Message &msg, std::string const& topic);


	std::string const getEndPointName() const
	{
		return endpoint_name;
	}

	MessageBuffer *getWriteBuffer() const
	{
		return write_buf;
	}

	std::vector<msgentry> remove();

	MessageBuffer *getReadBuffer() const
	{
		return read_buf;
	}

	void updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::size_t host = msgentry::HOST_ID_LOCAL_HOST);
	template<typename M> void attachTo(M& m)
	{
		read_buf = m.makeReadBuffer(endpoint_name);
		write_buf = m.makeWriteBuffer(endpoint_name);
	}

private:
	std::string endpoint_name;
	MessageBuffer *write_buf, *read_buf;


};

#endif /*ENDPOINT_H */
