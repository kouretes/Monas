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

#include "image_sender.h"
#include "image.pb.h"

void ImageSender::run()
{
    static int counter =0;
// 		cout << "A" << endl;
		static int resx =6555;
		static int resy =1;
		int t = 0;
		Image im;
// 		std::string s("a");
// 		im.set_timeout(100);
// 		im.set_topic("global");
// 		im.set_resx(resx);
// 		im.set_resy(resy);
// 		for(int i = 0; i < resx*resy-1;i++)
// 			im.add_image(s);
		publish(&im);
// 		std::cin >> t;
}

void  ImageSender::publish(google::protobuf::Message* msg)
{
	
	static int counter = 0;
	Publisher::publish(msg);
	counter++;
	if(counter == 100)
		Thread::stop();
}

