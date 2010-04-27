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

#include "scorekeeper.h"
#include "pingpong.pb.h"
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;


void ScoreKeeper::process_messages()
{
    //Subscriber::process_messages();
      static int delivered = 0;
        MessageBuffer* sub_buf = Subscriber::getBuffer();
    cout << "SCOREKEEEPER " << endl;
    if(sub_buf == NULL)
      cout << "None Unprocessed Buffers" << endl;
    google::protobuf::Message*  cur = sub_buf->remove_head();
  
  
    while(cur != NULL )
    {
//       if(cur->GetTypeName() ==  "PingMessage"  )
//       {
// 	PingMessage* msg = (PingMessage*)cur;
// 	msg->successful() == 1 ? pong++ : pong;
// // 	cout << "Message received: ping" << delivered << endl;
//       }
//        else if(cur->GetTypeName() ==  "PongMessage")
//       {
// 	PongMessage* msg = (PongMessage*)cur;
// 	msg->successful() == 1 ? ping++ : pong;
// // 	cout << "Message received: pong" << delivered << endl;
//       }
    if(cur->GetTypeName() != "a")
    {
      PongMessage* msg = (PongMessage*)cur;
      if(msg->successful())
	if(cur->GetTypeName() == "PingMessage")
	  pong++;
	
	else
	{
// 	  cout << "Pong " << msg->foo() << endl;
	  ping++;
	}
    }
      else
	cout << "Unknown Type " << cur->GetTypeName();
      delete cur; 
      cur = sub_buf->remove_head();
      if(delivered++ == 200)
	Thread::stop();
    }
    
    cerr << "Score is: " << endl;
    cerr <<  "PING: " << ping << " - " << " PONG: " << pong << endl;
    
 
}

void ScoreKeeper::publish ( google::protobuf::Message* msg )
{
    Publisher::publish ( msg );
}

void ScoreKeeper::run()
{
//  cout << "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" << endl;
  if(Subscriber::getBuffer() != NULL )
    if ( Subscriber::getBuffer()->size() > 0)
    process_messages();
}

