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

#include "button_listener_module.h"

#include <string>
using std::string;
using AL::ALValue;
using std::cout;
using std::endl;
ButtonListener::ButtonListener(AL::ALPtr<AL::ALBroker> pbroker, const std::string& name) : AL::ALModule(pbroker,name)/* bool* button_pressed, bool* left_bumper_pressed, bool* right_bumper_pressed, Mutex* mx) :AL::ALModule(pbroker,string("ButtonListener")),
                             chest_button_pressed(button_pressed),left_bumper_pressed(left_bumper_pressed),
                             right_bumper_pressed(right_bumper_pressed),mx(mx)*/
{
    setModuleDescription( "Module that listens button events" );
    functionName("buttonPressed","ButtonListener", "button pressed");
    BIND_METHOD(ButtonListener::buttonPressed);
    functionName("LBumperPressed","ButtonListener", "left bumber pressed");
    BIND_METHOD(ButtonListener::LBumperPressed);
    functionName("RBumperPressed","ButtonListener", "right bumber pressed");
    BIND_METHOD(ButtonListener::RBumperPressed);
    cout << "Button Listener start of constructor" << endl;
#include <boost/date_time/posix_time/posix_time_types.hpp>
		ctime = boost::posix_time::microsec_clock::local_time();
		rtime = boost::posix_time::microsec_clock::local_time();
		ltime = boost::posix_time::microsec_clock::local_time();
    try {
        sentinel = getParentBroker()->getProxy("ALSentinel");
        memory  = getParentBroker()->getMemoryProxy();
      }
    catch( AL::ALError& e) {
           cout << "Could not create sentiner or memory in ButtonListener Module " << endl;
      }
      //disable default chest button function
        sentinel->pCall<bool>("enableDefaultActionSimpleClick",false);
        //subscribe on chest button pressed
        memory->subscribeToEvent(string("ChestButtonPressed"),getName(),string("buttonPressed"));
        memory->subscribeToEvent (
                    "LeftBumperPressed",
                    getName(),

                    "LBumperPressed" );

          memory->subscribeToEvent(
                    "RightBumperPressed",
                    getName(),

                    "RBumperPressed" );
            cout << "Button Listener end of constructor" << endl;
	    memory->insertData("button_pressed",0);
	    memory->insertData("rbumper_pressed",0);
	    memory->insertData("lbumper_pressed",0);
}

ButtonListener::~ButtonListener()
{
sentinel->pCall<bool>("enableDefaultActionSimpleClick",true);
}

void ButtonListener::buttonPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage)
{
	 static boost::posix_time::time_duration dur = boost::posix_time::millisec(300);
	 boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
	 
   // mx->Lock();
		if((now - ctime) > dur )
		{
			cout << "Button Pressed" << endl;
			memory->insertData("button_pressed",1);
			ctime = now;
		}
		else
			cout << "ignore event" << endl;
   // mx->Unlock();
    //memory->raiseEvent("LBumperPressed",1.0f);
}
void ButtonListener::LBumperPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage)
{
	static boost::posix_time::time_duration dur = boost::posix_time::millisec(300);
	 boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

   // mx->Lock();
		if((now - ctime) > dur )
		{
			 cout << "Left Pressed" << endl;
			 memory->insertData("lbumper_pressed",1);
		}
		else
			cout << "ignore event" << endl;
}
void ButtonListener::RBumperPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage)
{
	static boost::posix_time::time_duration dur = boost::posix_time::millisec(300);
	 boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

   // mx->Lock();
		if((now - ctime) > dur )
		{
			   cout << "Right Pressed" << endl;
				memory->insertData("rbumper_pressed",1);
		}
		else
			cout << "ignore event" << endl;
}

