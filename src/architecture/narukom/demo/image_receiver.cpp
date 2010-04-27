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

#include "image_receiver.h"
#include "image.pb.h"

void ImageReceiver::run()
{
// 	cout << "B" << endl;
    if(Subscriber::getBuffer() != NULL)
			if(Subscriber::getBuffer()->size() > 0)
				process_messages();
}

void ImageReceiver::process_messages()
{
	static int counter  =0 ;
    MessageBuffer* sub_buf = Subscriber::getBuffer();
		Image* im = (Image*)sub_buf->remove_head();
		while(im != 0)
		{
			counter++;
 			
			im = (Image*)sub_buf->remove_head();
		}
		if(counter == 100)
			Thread::stop();
}

