#include "LedHandler.h"
#include "core/architecture/messaging/MessageBuffer.hpp"
#include "hal/robot/nao/generic_nao/kAlBroker.h"
#include "core/architecture/time/SystemTime.hpp"
#include "tools/toString.h"

using std::string;
using namespace KSystem::Time;

PROVIDER_REGISTER(LedHandler);

void LedHandler::UserInit()
{
	//led_change = 0;

	updateSubscription("leds", msgentry::SUBSCRIBE_ON_TOPIC);
	EndPoint::getReadBuffer()->setNotifier(boost::bind(&LedHandler::processBuffer, this, _1));
	IdlingThread::setIdling(true);



	try
	{
		leds = KAlBroker::Instance().GetBroker()->getProxy("ALLeds");
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Could not create ALLEds Proxy";
	}

	try
	{
		memory = KAlBroker::Instance().GetBroker()->getMemoryProxy();
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Error in getting ALmemory proxy";
	}

	initializeColorMap();
	//	setChestColor("on");
	//	setFootColor("r_foot", "on");
	//	setFootColor("l_foot", "on");
	left_ear_names.push_back("Ears/Led/Left/0Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/36Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/72Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/108Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/144Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/180Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/216Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/252Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/288Deg/Actuator/Value");
	left_ear_names.push_back("Ears/Led/Left/324Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/0Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/36Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/72Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/108Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/144Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/180Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/216Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/252Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/288Deg/Actuator/Value");
	right_ear_names.push_back("Ears/Led/Right/324Deg/Actuator/Value");

	try
	{
		battery_level = memory->getData("Device/SubDeviceList/Battery/Charge/Sensor/Value");
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,GetName()) << "Error somewhere here";
	}

	leds->callVoid<string> ("off", "AllLeds");
	LogEntry(LogLevel::Info,GetName()) <<"Initialized";

}

void LedHandler::Reset(){
}

int LedHandler::Execute()
{
	std::vector<msgentry> msg = EndPoint::remove();
	std::vector<msgentry>::iterator it = msg.begin();

	for(; it != msg.end(); ++it)
	{
		if((*it).msg->GetTypeName()=="LedChangeMessage")
		{
			led_change=boost::static_pointer_cast<const LedChangeMessage>( (*it).msg);

			for (int i = 0; i < led_change->leds_size(); i++)
			{
				setLed(led_change->leds(i).chain(), led_change->leds(i).color());
			}
		}

	}

	SetBatteryLevel();
	IdlingThread::idleThread();
	return 0;
}


void LedHandler::SetBatteryLevel()
{
	static bool firstRun = true;
	if (firstRun)
	{
		LogEntry(LogLevel::Info,GetName()) << "Real Battery level: " <<(battery_level);
		//10 == empty , 0, == full
		battery_level = rint((1 - battery_level) * left_ear_names.size()); //scale and reverse the real value

		if(battery_level >= left_ear_names.size())
			battery_level = left_ear_names.size() - 1;

		LogEntry(LogLevel::Info,GetName()) << "Reversed Battery level: " << (battery_level);

		firstRun = false;
	}

	float new_battery_level = memory->getData("Device/SubDeviceList/Battery/Charge/Sensor/Value");
	//10 == empty , 0, == full
	new_battery_level = rint((1 - new_battery_level) * left_ear_names.size()); //scale and reverse the real value

	if(new_battery_level >= left_ear_names.size())
		new_battery_level = left_ear_names.size() - 1;

	//Logger::Instance().WriteMsg("LedHandler", "Battery Level: "+_toString(new_battery_level), Logger::ExtraExtraInfo);

	if(new_battery_level > battery_level )
	{
		//Discharging so light up the next led
		LogEntry(LogLevel::ExtraExtraInfo,GetName()) << "Discharging, Battery Level: " << (new_battery_level);
		leds->callVoid<string>("on", left_ear_names[new_battery_level]);

		if(battery_level > 0)
			leds->callVoid<string>("on", left_ear_names[battery_level - 1]);
	}
	else if(new_battery_level < battery_level)
	{
		//Charging so light up the next led
		LogEntry(LogLevel::ExtraExtraInfo,GetName()) << "Charging, Battery Level: " << (new_battery_level);
		leds->callVoid<string>("off", left_ear_names[battery_level]);
	}

	battery_level = new_battery_level;
	static ptime last_ledchange = KSystem::Time::SystemTime::now();

	if(KSystem::Time::SystemTime::now() - last_ledchange < milliseconds(2000 / (1 + battery_level)))
		return ;

	last_ledchange = KSystem::Time::SystemTime::now();
	static bool ledonoff = false;
	ledonoff = !ledonoff;
	for(unsigned int i = 0; i < left_ear_names.size(); i++)
	{
		leds->callVoid<string>("off", right_ear_names[i]);
		if(i < battery_level)
			leds->callVoid<string>("on", left_ear_names[i]);
		else if(i == battery_level)
			leds->callVoid<string>( (ledonoff) ? "on" : "off", left_ear_names[i] );
		else
			leds->callVoid<string>("off", left_ear_names[i]);
	}
}



void LedHandler::setLed(const string& device, const string& color)
{
	if (device.compare("chest") == 0)
	{
		setChestColor(color);
		return;
	}

	if (device.compare("l_foot") == 0)
	{
		setFootColor(device, color);
		return;
	}

	if (device.compare("r_foot") == 0)
	{
		setFootColor(device, color);
		return;
	}

	if ((device.compare("r_ear") == 0) || (device.compare("l_ear") == 0))
	{
		setIndividualEarColor(device, color);
		return;
	}

	if ((device.compare("r_eye") == 0) || (device.compare("l_eye") == 0))
	{
		setIndividualEyeColor(device, color);
		return;
	}

	if (device.compare("eyes") == 0)
	{
		setEyesColor(color);
		return;
	}

	if (device.compare("ears") == 0)
	{
		setEarsColor(color);
		return;
	}
}


void LedHandler::setChestColor(const string& color)
{
	leds->callVoid<string, int, float> ("fadeRGB", "ChestLeds", colors[color], 0.0);
}


void LedHandler::setFootColor(const string& device, const string& color)
{
	if (device.find("l") != device.npos)
	{
		leds->callVoid<string, int, float> ("fadeRGB", "LeftFootLeds", colors[color], 0.0);
	}
	else
	{
		leds->callVoid<string, int, float> ("fadeRGB", "RightFootLeds", colors[color], 0.0);
	}
}


void LedHandler::setEyesColor(const string& color)
{
	leds->callVoid<string, int, float> ("fadeRGB", "FaceLeds", colors[color], 0.0);
}


void LedHandler::setIndividualEyeColor(const string& device, const string& color)
{
	if (device.find("l") != device.npos)
	{
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftExternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftInternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftBottom", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsLeftTop", colors[color], 0.0);
	}
	else
	{
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightExternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightInternal", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightBottom", colors[color], 0.0);
		leds->callVoid<string, int, float> ("fadeRGB", "FaceLedsRightTop", colors[color], 0.0);
	}
}


void LedHandler::setEarsColor(const string& color)
{
	if (color.compare("off") != 0 && color.compare("blue") != 0)
	{
		return;
	}

	leds->callVoid<string, int, float> ("fadeRGB", "EarLeds", colors[color.c_str()], 0.0);
}


void LedHandler::setIndividualEarColor(const string& device, const string& color)
{
	if (color.compare("off") != 0 && color.compare("blue") != 0)
	{
		//cout << "Invalid color for Ears \n Please use only blue and off" << endl;
		return;
	}

	if (device.find("l") != device.npos)
		leds->callVoid<string, int, float> ("fadeRGB", "LeftEarLeds", colors[color.c_str()], 0.0);
	else
		leds->callVoid<string, int, float> ("fadeRGB", "RightEarLeds", colors[color.c_str()], 0.0);
}


void LedHandler::initializeColorMap()
{
	//add off to color map
	colors["off"] = 0x00000000;
	//add on = white to color map
	colors["on"] = 0x00FFFFFF;
	//add red to color map
	colors["red"] = 0x00FF0000;
	//add green to color map
	colors["green"] = 0x0000FF00;
	//add blue to color map
	colors["blue"] = 0x000000FF;
	//add yellow to color map
	colors["yellow"] = 0x00FFE000;//FFF52C;
	//add orange to color map
	colors["orange"] = 0x00FFFF00;
	colors["purple"] = 0x00FF00FF;
}


int LedHandler::getColor(string color)
{
	return colors[color];
}
