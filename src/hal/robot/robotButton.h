#ifndef _robot_button_h
#define _robot_button_h 1

#include<string>
#include"robot_sensor.h"

class RobotButton : public RobotSensor
{


public:

	RobotButton(std::string name) :
		RobotSensor(name)
	{
		;
	}

	isPressed() = 0;

};

#endif // _robot_button_h
