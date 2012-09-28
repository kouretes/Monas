#ifndef KalmanWorldState_H
#define KalmanWorldState_H
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pthread.h>
#include <netinet/in.h>
#include <math.h>
#include <csignal>

#include "architecture/executables/IActivity.h"
#include "architecture/archConfig.h"


#include "hal/robot/generic_nao/robot_consts.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/motion.pb.h"

#include "BallFilter.h"
#include "KLocalization.h"


ACTIVITY_START
class KalmanWorldState: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(KalmanWorldState)
	ACTIVITY_VISIBLE ~KalmanWorldState()
	{

	}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void process_messages();
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "KalmanWorldState";
	}

	void RobotPositionMotionModel(KMotionModel & MModel);
	void calculate_ball_estimate(KMotionModel const & MModel);

private:
	//check if the first odometry data had come
	bool firstOdometry;

	//WorldInfo message
	WorldInfo MyWorld;

	//Current agent position
	belief AgentPosition;


	KLocalization localizationWorld;
	//Observations and odometry data to feed localization
	vector<KObservationModel> currentObservation;
	vector<KObservationModel> currentAmbiguousObservation;
	KMotionModel robotmovement;

	//Use a particle to store the odometry data
	partcl TrackPoint;
	partcl TrackPointRobotPosition;



	//Store the odometry data from the sensors message
	SensorData PosX;
	SensorData PosY;
	SensorData Angle;

	//Our ball filter
	BallFilter myBall;

	//Message we need to read
	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const RobotPositionMessage> rpsm;
	boost::shared_ptr<const LocalizationResetMessage> lrm;
	boost::shared_ptr<const MotionStateMessage> sm;
	MotionStateMessage::ActionType currentRobotAction;

	//Check if fall of the robot just began
	bool fallBegan;

	//Time variables
	boost::posix_time::ptime timeStart, timeStop;
	boost::posix_time::ptime last_observation_time;
	boost::posix_time::ptime last_filter_time;
	boost::posix_time::ptime now;

};
ACTIVITY_END
#endif
