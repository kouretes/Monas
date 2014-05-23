#ifndef XARACTION_H
#define XARACTION_H

#include "ISpecialAction.h"

#include <string>

#include "hal/robot/nao/generic_nao/aldebaran-motion-extra.h"

class XarAction: public ISpecialAction
{

public:

	std::string GetName()
	{
		return name;
	}
	//TODO fix this constructor
	XarAction(AL::ALFrameManagerProxy *frame, std::string filenamepath, std::string name, std::string identifier);

	int ExecutePost();

private:

	int ExecuteActionXAR();

	//int ExecuteActionBodyXAR();

	//     int PrintActionsXAR();

	std::string name;
	std::string filename;
	std::string identifier;

	//boost::shared_ptr<AL::ALFrameManagerProxy> frame;
	AL::ALFrameManagerProxy *frame;
};

#endif // _XARACTION_H_
