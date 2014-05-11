#include "KmexAction.h"

#include "hal/robot/nao/generic_nao/kAlBroker.h"
#include "hal/robot/nao/generic_nao/robot_consts.h"
#include "core/include/Logger.hpp"
using namespace std;

KmexAction::KmexAction( string name, int numberOfPoses, vector<int> poses, int totalPoses, float threshold, int numberOfJoints,
                        vector<int> joints, vector<vector <float> > angles, vector<vector <float> > anglesVelocity )
{
	try
	{
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	}
	catch (AL::ALError& e)
	{
		LogEntry(LogLevel::FatalError,"KmexAction") << "Error in getting motion proxy";
	}

	this->name = name;
	this->numberOfPoses = numberOfPoses;
	this->poses = poses;
	this->totalPoses = totalPoses;
	this->threshold = threshold;
	this->numberOfJoints = numberOfJoints;
	this->joints = joints;
	this->angles = angles;
	this->anglesVelocity = anglesVelocity;
	DcmInit();
}

void KmexAction::DcmInit()
{
	walkPrevAng.assign(numberOfJoints, 0);
}

vector<int> KmexAction::AngleCompare(boost::shared_ptr<const AllSensorValuesMessage> allsm)
{
	float diffW[numberOfJoints];
	vector<int> frames;
	float norm[numberOfPoses];
	float tempA[numberOfPoses];
	float tempB[numberOfPoses];

	for ( int j = 0; j < numberOfPoses; j++ )
	{
		tempA[j] = 0.0;
		tempB[j] = 0.0;
	}

	float walkAngles[numberOfJoints];

	for (int i = 0; i < numberOfJoints; i++)
	{
		walkAngles[i] = allsm->jointdata(joints[i]).sensorvalue();
	}

	for (int i = 0; i < numberOfJoints; i++)
	{
		diffW[i] = walkAngles[i] - walkPrevAng[i];
	}

	for (int j = 0; j < numberOfPoses; j++)
	{
		for (int i = 0; i < numberOfJoints; i++)
		{
			tempA[j] = pow((angles[j][i] - walkAngles[i]), 2) + tempA[j];
			tempB[j] = pow((anglesVelocity[j][i] - diffW[i]), 2) + tempB[j];
			walkPrevAng[i] = walkAngles[i];
		}
	}

	int ready = 0;
	int frameStart = 0;

	for (int j = 0; j < numberOfPoses; j++)
	{
		norm[j] = sqrt(tempA[j] + tempB[j]);

		//Logger::Instance().WriteMsg("KmexAction", "norm : " +  _toString(norm[j]), Logger::ExtraInfo);
		if (norm[j] <= threshold)
		{
			frameStart = poses[j];
			ready = 1;
		}
		else

			LogEntry(LogLevel::ExtraInfo,"KmexAction") << "MPA ";
	}

	if (ready == 1)
	{
		frames.push_back(frameStart);
		frames.push_back(totalPoses);
	}

	return frames;
}

