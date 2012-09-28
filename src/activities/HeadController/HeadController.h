#ifndef HeadController_H
#define HeadController_H

#include "architecture/executables/IActivity.h"


#include "messages/SensorsMessage.pb.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/Kimage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "architecture/archConfig.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#define PITCHMIN -0.55
#define	PITCHMAX 0.33
#define YAWMIN 0.8
#define YAWMAX 1.35
#define PITCHSTEP 0.3
#define YAWSTEP 0.4

#define OVERSH 0.1
#define WAITFOR 40


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
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

	void read_messages();

	void GetGameState();
	void GetPosition();
	void CheckForBall();

	int MakeHeadAction();

	void HeadScanStepHigh(float yaw_limit);
	void HeadScanStepSmart();
	void HeadTrackIntelligent();
	float lookAtPointYaw(float x, float y);
	float lookAtPointPitch(float x, float y);
	float lookAtPointRelativeYaw(float x, float y);
	float lookAtPointRelativePitch(float x, float y);

	bool reachedTargetHead()
	{
		std::cout<<"t:"<<targetPitch<<" "<<targetYaw<<std::endl;
		std::cout<<"s:"<<HeadPitch.sensorvalue()<<" "<<HeadYaw.sensorvalue()<<std::endl;
		return  (fabs(targetPitch - HeadPitch.sensorvalue()) <= OVERSH) && (fabs(targetYaw - HeadYaw.sensorvalue()) <= OVERSH) ;
	}

private:

	static const double closeToBall = 1.3;


	void calibrate();

	bool readConfiguration(const std::string& file_name); 		//this function reads team's configuration info from XML file
	bool readRobotConfiguration(const std::string& file_name); 	//this function reads robot's initial position in the field from XML file
	bool readGoalConfiguration(const std::string& file_name); 	//this function reads the position of the goals

	float dist(float x1, float y1, float x2, float y2);

	/* Incoming Messages */
	boost::shared_ptr<const HeadControlMessage> control;
	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const BallTrackMessage>  bmsg;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const WorldInfo>  wim;
	boost::shared_ptr<const SharedWorldInfo>  swim;

	/* Outgoing Messages */
	MotionHeadMessage hmot;

	int leftright;
	float headpos;

	short seeballmessage,seeballtrust;

	bool scanforball;
	bool startscan;
	bool scanOK;
	bool pathOK;
	float targetYaw;
	float targetPitch;
	SensorData HeadYaw;
	SensorData HeadPitch;
	float psign, ysign;
	unsigned waiting;
	int calibrated;

	float bd, bb, bx, by;
	float robot_x, robot_y, robot_phi, robot_confidence;

	int gameState;
	int teamColor;
	int playerNumber;
	bool readRobotConf;
	//POSITIONS OF THE GOALPOSTS ON THE FIELD, LOADED FROM XML
	float 	oppGoalX ,oppGoalY,ownGoalY,ownGoalX,
			oppGoalLeftX,oppGoalLeftY,ownGoalLeftX,
			ownGoalLeftY,	oppGoalRightX ,
			ownGoalRightX ,ownGoalRightY ,oppGoalRightY;


	boost::posix_time::ptime lastmove, lastball, lastwalk, lastplay, lastpenalized,  lastrolechange;
};

ACTIVITY_END
#endif
