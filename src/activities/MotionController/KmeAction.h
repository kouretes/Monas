#ifndef KMEACTION_H
#define KMEACTION_H

#include "ISpecialAction.h"

#include <string>
#include "core/architecture/time/TimeTypes.hpp"
#include "hal/robot/nao/generic_nao/aldebaran-kme.h"

class KmeAction : public ISpecialAction
{

public:

	std::string GetName ()
	{
		return name;
	}

	KmeAction(std::string name, AL::ALValue actionNames,
	          AL::ALValue actionAngles, AL::ALValue actionTimes );

	int ExecutePost();
	AL::ALValue ReturnALValues();
	KSystem::Time::TimeAbsolute ExecuteDCM();

	KSystem::Time::TimeAbsolute ExecuteFrameDCM(unsigned int frameStart, unsigned int frameEnd);

private:

	int ExecuteActionKME();

	int ExecuteActionBodyKME();

	void DcmInit();


	//     int PrintActionsKME();
	AL::ALValue commands;

	std::string name;

	boost::shared_ptr<AL::ALMotionProxy> motion;
	AL::DCMProxy *dcm;

	AL::ALValue actionNames, actionAngles, actionTimes;

};

#endif // _KMEACTION_H_
