#ifndef HEADCONTROLLER_H
#define HEADCONTROLLER_H

#include <math.h>

#include "core/include/IActivity.hpp"
#include "core/elements/math/Common.hpp"
#include "core/architecture/configurator/Configurator.hpp"

#include "hal/robot/nao/generic_nao/robot_consts.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/Motion.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Kimage.pb.h"
#include "messages/WorldInfo.pb.h"


#include "core/include/Logger.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>


ACTIVITY_START
class HeadController: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(HeadController);
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "HeadController";
	}
	void ACTIVITY_VISIBLE  UserInit();
	void ACTIVITY_VISIBLE  Reset();
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

private:
	static const double closeToBall = 1.5f;
	static const float headSpeed[3];/*{SOMEDAY, SLOW, BEAM_ME_UP}*/
	static const float PITCHMIN = -0.67f;
	static const float PITCHMAX = 0.51f;
	static const float YAWMAX = 2.086f;
	static const float OVERSH = 0.1f;

	unsigned int currentCommand, previousCommand;

	enum{
		SLOW = 0,
		NORMAL = 1,
		FAST = 2
	};
	enum GOALSCANSTATE{
		GOALMIDDLE1 = 0,
		GOALRIGHT = 1,
		GOALMIDDLE2 = 2,
		GOALLEFT
	};
	enum INTELSTATE{
		BALL1 = 0,
		OPPG = 1,
		BALL2 = 2,
		OWNG
	};
	enum SMARTPHASE{
		BLUE = 0,
		RED = 1,
		GREEN
	};
	enum SMARTSTATE{
		START = 0,
		MIDDLE = 1,
		END
	};
	INTELSTATE intelState;
	SMARTSTATE smartState;
	SMARTPHASE smartPhase;
	GOALSCANSTATE goalScanState;
	void readGoalConfiguration();//this function reads the position of the goals

	/* Incoming Messages */
	boost::shared_ptr<const HeadControlMessage> control;
	boost::shared_ptr<const AllSensorValuesMessage>	allsm;
	boost::shared_ptr<const WorldInfo> wim;
	boost::shared_ptr<const SharedWorldInfo> swim;

	/* Outgoing Messages */
	MotionHeadMessage hmot;

	/* Current joint values */
	float currentHeadYaw, currentHeadPitch;

	/* Target joint values */
	float targetYaw, targetPitch, targetSpeed;
	/* Find out if an action take a lot of time to complete */
	boost::posix_time::ptime actionStarted;
	static const int millisecondsToWait = 1500;

	/* Sleep time just to stay to a location */
	boost::posix_time::ptime sleepTime;
	bool stay;
	/* Force completition of all actions */
	bool forceStopMotion;
	/* Value to describe where to look with smart scan (or something like that) */
	float ysign;

	/* Ball values from world info */
	float bd, bb, bx, by;

	/* Robot values from world info */
	float robotX, robotY, robotPhi, robot_confidence;

	/* External speed controler (e.g. from KMonitor) */
	bool useExternalSpeed;
	float externalSpeed;
	/* Positions fo the GoalPosts on the field */
	float oppGoalX, oppGoalY, ownGoalY, ownGoalX;
	float oppGoalLeftX, oppGoalLeftY, ownGoalLeftX, ownGoalLeftY;
	float oppGoalRightX, oppGoalRightY, ownGoalRightX, ownGoalRightY;

	/*Read messages from blackboard*/
	void ReadMessages();

	/*Read the position of the robot*/
	void GetPosition();

	/*Check if vision observasion message has ball*/
	bool CheckForBall();

	/*Send the message with the head action*/
	void MakeHeadAction();

	/*Scan high for goalposts*/
	void HeadScanStepHigh(float yawLimit, float pitch, int speed, int millesecStat);

	/*Scan for the ball*/
	void HeadScanStepSmart(int speed);

	/*Track the ball and search for goalposts*/
	void HeadTrackIntelligent();

	float lookAtPointYaw(float x, float y);
	float lookAtPointPitch(float x, float y);
	float lookAtPointRelativeYaw(float x, float y);
	float lookAtPointRelativePitch(float x, float y);

	/*Target head possition reached*/
	bool reachedTargetHead();
};
ACTIVITY_END
#endif
