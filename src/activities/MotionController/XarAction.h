#ifndef _XARACTION_H_
#define _XARACTION_H_ 1

#include "ISpecialAction.h"

#include <string>

#include "hal/robot/generic_nao/aldebaran-motion-extra.h"

class XarAction: public ISpecialAction
{

public:

	std::string GetName()
	{
		return name;
	}
	//TODO fix this constructor
	XarAction(AL::ALPtr<AL::ALFrameManagerProxy> frame, std::string filenamepath, std::string name, std::string identifier);

	int ExecutePost();

private:

	int ExecuteActionXAR();

	//int ExecuteActionBodyXAR();

	//     int PrintActionsXAR();

	std::string name;
	std::string filename;
	std::string identifier;

	//AL::ALPtr<AL::ALFrameManagerProxy> frame;
	AL::ALPtr<AL::ALFrameManagerProxy> frame;
};

#endif // _XARACTION_H_
