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

#include "echo.h"
#include <iostream>
#include <google/protobuf/descriptor.h>

using std::cout;
using std::endl;
int Echo::Execute()
{
  if(Subscriber::getBuffer() != NULL)
  { 
    if(Subscriber::getBuffer()->size() > 0)
    ;// process_messages();
  }
  return 0;
}

void Echo::process_messages()
{
  static int  delivered  = 0;
    //Subscriber::process_messages();
    MessageBuffer* sub_buf = Subscriber::getBuffer();
    cout << "Echoer " << endl;
    if(sub_buf == NULL)
      cout << "None Unprocessed Buffers" << endl;
    Tuple*  tcur = sub_buf->remove_head();

    while(tcur != 0 )
    {
    //  cout << " current size = " << sub_buf->size() << endl;;
      
      cout << "Received Message  from: "  << tcur->get_publisher() << "   " << tcur << endl;
      cout << "Topic: " << tcur->get_topic() << "  Type: " << tcur->get_type() <<endl;
      delete tcur;
      tcur = sub_buf->remove_head();
      if(delivered++ == 20)
	Thread::StopThread();
      if(tcur == NULL)
	break;
      
    }
//     sleep(20);
}

