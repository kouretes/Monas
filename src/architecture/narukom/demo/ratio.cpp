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

#include "ratio.h"

void Ratio::run()
{
 	  
    b.process_messages();
		cout << "Ratio Run" << endl;
		SyncMessage* tmp = (SyncMessage*)b.in_nb("SyncMessage","Slow","localhost");
		if(tmp != 0){
			cout << "1234" << endl;
 			boost::posix_time::ptime query = boost::posix_time::from_iso_string(tmp->timestamp());
 				cout << "Ratio Run2" << endl;
 			SyncMessage* tmp2 = (SyncMessage*)b.read_nb("SyncMessage","Fast","localhost",&query);
 				cout << "Ratio Run3" << endl;
			if(tmp2 != NULL)
			cout << ""  <<  tmp2->counter() <<"," << tmp->counter() << "," << tmp2->counter() / (float)tmp ->counter() << endl;
		}
 		usleep(500000);
}

