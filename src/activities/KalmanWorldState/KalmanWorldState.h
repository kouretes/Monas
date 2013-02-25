#ifndef KalmanWorldState_H
#define KalmanWorldState_H
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pthread.h>
#include <netinet/in.h>
#include <math.h>
#include <csignal>

#include "core/include/IActivity.hpp"
//#include "core/architecture/archConfig.h"


#include "hal/robot/generic_nao/robot_consts.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/motion.pb.h"

#include "BallFilter.h"
#include "KKalmanLocalization.h"


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
	void ACTIVITY_VISIBLE Reset();
	void process_messages();
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "KalmanWorldState";
	}

	void RobotPositionMotionModel(KKalmanLocalization::KMotionModel & MModel);
	void calculate_ball_estimate(KKalmanLocalization::KMotionModel const & MModel);
private:
	//check if the first odometry data had come
	bool firstOdometry;

	//WorldInfo message
	WorldInfo MyWorld;

	//Current agent position
	KKalmanLocalization::belief AgentPosition;


	KKalmanLocalization localizationWorld;
	//Observations and odometry data to feed localization
	vector<KKalmanLocalization::KObservationModel> currentObservation;
	vector<KKalmanLocalization::KObservationModel> currentAmbiguousObservation;
	KKalmanLocalization::KMotionModel robotmovement;

	//Use a particle to store the odometry data
	KKalmanLocalization::partcl TrackPoint;
	KKalmanLocalization::partcl TrackPointRobotPosition;



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

	//Read configurations
    void ReadLocConf();
    void ReadFieldConf();
    void ReadFeatureConf();
    void ReadTeamConf();
    void ReadRobotConf();
	//Time variables
	boost::posix_time::ptime timeStart, timeStop;
	boost::posix_time::ptime last_observation_time;
	boost::posix_time::ptime last_filter_time;
	boost::posix_time::ptime now;

};
ACTIVITY_END
#endif
