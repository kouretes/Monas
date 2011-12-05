#include "KmexAction.h"

#include "hal/robot/generic_nao/kAlBroker.h"
#include "hal/robot/generic_nao/robot_consts.h"

using namespace std;

KmexAction::KmexAction( string name, int numberOfPoses, vector<int> poses, int totalPoses, float threshold, int numberOfJoints,
		vector<int> joints, vector<vector <float> > angles, vector<vector <float> > anglesVelocity ) {

	try {
		motion = KAlBroker::Instance().GetBroker()->getMotionProxy();
	} catch (AL::ALError& e) {
		Logger::Instance().WriteMsg("KMEAction", "Error in getting motion proxy", Logger::FatalError);
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

void KmexAction::DcmInit() {
	walkPrevAng.assign(numberOfJoints, 0);
}

vector<int> KmexAction::AngleCompare(boost::shared_ptr<const AllSensorValuesMessage> allsm)
{
	float CompAng[numberOfPoses][numberOfJoints];
	float diffKick[numberOfPoses - 1][numberOfJoints];
	float diffW[numberOfJoints];
	vector<int> frames;
	vector <float> anglesTemp (numberOfJoints, 0);
	vector <float> anglesVelTemp;
	float norm[numberOfPoses-1];

	for (int i = 0; i < numberOfJoints; i++)
	{
		diffW[i] = 0.0;
	}

	int k = 0;
	for(int j = 0; j < numberOfPoses; j++)
	{
		anglesTemp = angles[j];
		for (int i = 0; i < numberOfJoints; i++)
		{
			CompAng[j][i] = anglesTemp[k+i];
		}
		k = k + numberOfJoints;
	}

	int l=0;
	for(int j = 0; j < numberOfPoses-1; j++)
	{
		anglesVelTemp = anglesVelocity[j];
		for (int i = 0; i < numberOfJoints; i++)
		{
			diffKick[j][i] = anglesVelTemp[l+i];
		}
		l = l + numberOfJoints;
	}

	float tempA[numberOfPoses-1];
	float tempB[numberOfPoses-1];

	for(int j = 0; j < numberOfPoses-1; j++)
	{
			tempA[j] = 0;
			tempB[j] = 0;
	}

	float walkAngles[numberOfJoints];
	for( unsigned int i = 0; i<joints.size(); i++)
	{
		walkAngles[i] = allsm->jointdata(joints[i]).sensorvalue();
	}

	for (int i = 0; i < numberOfJoints; i++)
	{
		diffW[i] = walkAngles[i] - walkPrevAng[i];
	}


	for (int j = 0; j < numberOfPoses-1; j++)
	{
		for (int i = 0; i < numberOfJoints; i++)
		{
			tempA[j] = pow((CompAng[j][i] - walkAngles[i]),2) + tempA[j];
			tempB[j] = pow((diffKick[j][i] - diffW[i]),2) + tempB[j];

			walkPrevAng[i] = walkAngles[i];
		}
	}

	for (int j = 0; j < numberOfPoses-1; j++)
	{
		norm[j] = sqrt(tempA[j] + tempB[j]);
		Logger::Instance().WriteMsg("KmexAction", "norm : " +  _toString(norm[j]), Logger::ExtraInfo);
	}

	int ready = 0;
	int frameStart = 0;
	for (int j = 0; j < numberOfPoses-1; j++)
	{
		if (norm[j] <= threshold){
			frameStart = poses[j];
			ready = 1;
		}else
			Logger::Instance().WriteMsg("KmexAction", "MPA ", Logger::ExtraInfo);
	}
	if(ready == 1){
		frames.push_back(frameStart);
		frames.push_back(totalPoses);
	}

	return frames;
}

