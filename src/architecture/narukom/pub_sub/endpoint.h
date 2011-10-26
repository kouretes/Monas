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
    virtual void publish(  msgentry const& msg);
	virtual void publish(std::vector<msgentry> const& vec);

    std::string const getEndPointName() const { return endpoint_name;}

    MessageBuffer *getWriteBuffer() const { return write_buf;}

	std::vector<msgentry> remove();

	MessageBuffer *getReadBuffer() const { return read_buf;}

    void updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::string const& host="localhost");
    template<typename M> void attachTo(M& m) {

    	read_buf=m.makeReadBuffer(endpoint_name);
    	write_buf=m.makeWriteBuffer(endpoint_name);
    }

  private:
    std::string endpoint_name;
    MessageBuffer *write_buf,*read_buf;


};

#endif /*ENDPOINT_H */
