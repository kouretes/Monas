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
#include "tools/logger.h"
#include "tools/toString.h"
//#include "alsentinelproxy.h"

using AL::ALValue;

ButtonListener::ButtonListener(AL::ALPtr<AL::ALBroker> pbroker, const std::string& name) :
	AL::ALModule(pbroker, name){
	setModuleDescription("A Kouretes Module that listens button events");
	functionName("buttonPressed", "ButtonListener", "button pressed");
	BIND_METHOD(ButtonListener::buttonPressed);
	functionName("LBumperPressed", "ButtonListener", "left bumber pressed");
	BIND_METHOD(ButtonListener::LBumperPressed);
	functionName("RBumperPressed", "ButtonListener", "right bumber pressed");
	BIND_METHOD(ButtonListener::RBumperPressed);

	ctime = boost::posix_time::microsec_clock::universal_time();

	try {
		sentinel = getParentBroker()->getProxy("ALSentinel");
		memory = getParentBroker()->getMemoryProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("Button Listener Module ", "Could not create sentinel or memory Proxy", Logger::FatalError);
	}
	//disable default chest button function
	sentinel->pCall<bool> ("enableDefaultActionSimpleClick", false);
	//subscribe on chest button pressed
	memory->subscribeToEvent("ChestButtonPressed", getName(),"buttonPressed");
	memory->subscribeToEvent("LeftBumperPressed", getName(),"LBumperPressed");
	memory->subscribeToEvent("RightBumperPressed", getName(),"RBumperPressed");

	//cout << "Button Listener end of constructor" << endl;
	memory->insertData("button_pressed", 0);
	memory->insertData("rbumper_pressed", 0);
	memory->insertData("lbumper_pressed", 0);

	int milliseconds_events_ignore = 350;
	dur = boost::posix_time::millisec(milliseconds_events_ignore);
	Logger::Instance().WriteMsg("Button Listener Module", "Module Initialized, event ignore period: " + _toString(milliseconds_events_ignore) + "ms", Logger::Info);
}

ButtonListener::~ButtonListener() {
	sentinel->pCall<bool> ("enableDefaultActionSimpleClick", true);
}

void ButtonListener::buttonPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage) {
	now = boost::posix_time::microsec_clock::universal_time();

	if ((now - ctime) > dur) {
		ctime = now;
		//cout << "Button Pressed" << endl;
		memory->insertData("button_pressed", 1);
	} else
	{
		//cout << "ignore event" << endl;
		memory->insertData("button_pressed", 0);
	}

}
void ButtonListener::LBumperPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage) {
	now = boost::posix_time::microsec_clock::universal_time();

	if ((now - ctime) > dur) {
		ctime = now;
		//cout << "Left Pressed" << endl;
		memory->insertData("lbumper_pressed", 1);

	} else{
		//cout << "ignore event" << endl;
		memory->insertData("lbumper_pressed", 0);

	}
}
void ButtonListener::RBumperPressed(const std::string& pDataName, const ALValue& pValue, const std::string& pMessage) {
	now = boost::posix_time::microsec_clock::universal_time();

	if ((now - ctime) > dur) {
		ctime = now;
		//cout << "Right Pressed" << endl;
		memory->insertData("rbumper_pressed", 1);
	} else{
		//cout << "ignore event" << endl;
		memory->insertData("rbumper_pressed", 0);
	}
}

