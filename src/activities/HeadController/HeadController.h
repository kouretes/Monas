#ifndef HeadController_H
#define HeadController_H

#include <math.h>

#include "core/include/IActivity.hpp"

#include "hal/robot/generic_nao/robot_consts.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Kimage.pb.h"
#include "messages/WorldInfo.pb.h"

#include "tools/toString.h"
#include "tools/logger.h"
#include "core/elements/math/Common.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#define PITCHMIN -0.67
#define	PITCHMAX 0.51
#define YAWMIN 0.8
#define YAWMAX 1.9
#define PITCHSTEP 0.3
#define YAWSTEP 0.4

#define OVERSH 0.1
#define WAITFOR 15


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
	static const double closeToBall = 4.0;
	static const float headSpeed[3];/*{SOMEDAY, SLOW, BEAM_ME_UP}*/

	enum{
		SLOW = 0,
		NORMAL = 1,
		FAST = 2
	};
	enum INTELSTATE{
		BALL1 = 0,
		OPPG = 1,
		BALL2 = 2,
		OWNG
	};
	INTELSTATE state;
	void readGoalConfiguration();//this function reads the position of the goals

	/* Incoming Messages */
	boost::shared_ptr<const HeadControlMessage> control;
	boost::shared_ptr<const AllSensorValuesMessage>	allsm;
	boost::shared_ptr<const BallTrackMessage> bmsg;
	boost::shared_ptr<const WorldInfo> wim;
	boost::shared_ptr<const SharedWorldInfo> swim;

	/* Outgoing Messages */
	MotionHeadMessage hmot;
	BallFoundMessage bfm;

	short seeballmessage, seeballtrust;

	bool scanforball;
	bool startscan;
	float targetYaw;
	float targetPitch;
	float targetSpeed;
	float ysign;
	SensorData HeadYaw;
	SensorData HeadPitch;
	unsigned waiting;

	float bd, bb, bx, by;
	float robot_x, robot_y, robot_phi, robot_confidence;

	//External speed controler (e.g. from KMonitor)
	bool useExternalSpeed;
	float externalSpeed;;
	//POSITIONS OF THE GOALPOSTS ON THE FIELD, LOADED FROM XML
	float oppGoalX, oppGoalY, ownGoalY, ownGoalX;
	float oppGoalLeftX, oppGoalLeftY, ownGoalLeftX, ownGoalLeftY;
	float oppGoalRightX, oppGoalRightY, ownGoalRightX, ownGoalRightY;

	boost::posix_time::ptime lastball;

	/*Read messages from blackboard*/
	void read_messages();

	/*Read the position of the robot*/
	void GetPosition();

	/*Check if vision observasion message has ball*/
	void CheckForBall();

	/*Send the message with the head action*/
	int MakeHeadAction();

	/*Scan high for goalposts*/
	void HeadScanStepHigh(float yaw_limit);

	/*Scan for the ball*/
	void HeadScanStepSmart();

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
