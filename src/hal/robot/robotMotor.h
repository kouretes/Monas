#ifndef _robot_motor_h_
#define _robot_motor_h_ 1

#include "robot_effector.h"
#include "robot_sensor.h"

#include <string>

//TODO implement interpolation, speed etc

template<class MotorPositionType>
class RobotMotor : public RobotEffector, public RobotSensor
{

public:

	RobotMotor(std::string name, MotorPositionType from, MotorPositionType to)
		: RobotEffector(name), RobotSensor(name),
		  from(from), to(to)
	{
		;
	}

	~virtual RobotMotor()
	{
		;
	}

	MotorPositionType GetMotorPosition () = 0;

	void SetMotorPosition(MotorPositionType x) = 0;


private:

	MotorPositionType from, to;

};


#endif // _robot_motor_h_
