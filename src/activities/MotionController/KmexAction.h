#ifndef KMEXACTION_H
#define KMEXACTION_H

#include "ISpecialAction.h"

#include <string>
#include "hal/robot/nao/generic_nao/aldebaran-kme.h"
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "hal/robot/nao/generic_nao/aldebaran-sensors.h"
#include "messages/SensorsMessage.pb.h"

class KmexAction
{

public:

	std::string GetName ()
	{
		return name;
	}

	KmexAction( std::string name, int numberOfPoses, std::vector<int> poses, int totalPoses, float threshold, int numberOfJoints,
	            std::vector<int> joints, std::vector<std::vector <float> > angles, std::vector<std::vector <float> > anglesVelocity);

	std::vector<int> AngleCompare(boost::shared_ptr<const AllSensorValuesMessage> allsm);

private:

	void DcmInit();

	boost::shared_ptr<AL::ALMotionProxy> motion;

	std::string name;
	int numberOfPoses;
	std::vector<int> poses;
	int totalPoses;
	float threshold;
	int numberOfJoints;
	std::vector<int> joints;
	std::vector<std::vector <float> > angles;
	std::vector<std::vector <float> > anglesVelocity;

	std::vector<float> walkPrevAng;
};

#endif
