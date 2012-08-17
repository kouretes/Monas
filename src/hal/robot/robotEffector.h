#ifndef _robot_effector_h_
#define _robot_effector_h_ 1

#include<string>

class RobotEffector
{

public:

	RobotEffector(std::string &name) :
		name(name)
	{
		;
	}

	virtual ~RobotEffector()
	{
		;
	}

private:

	std::string name;

};

#endif // _robot_effector_h_
