#ifndef LocalWorldState_H
#define LocalWorldState_H

#include "architecture/executables/IActivity.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/motion.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BallFilter.h"
#include "KLocalization.h"
#include "PracticalSocket.h"
#include <string>


ACTIVITY_START
class LocalWorldState: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(LocalWorldState)
	ACTIVITY_VISIBLE ~LocalWorldState()
	{
		if (serverpid != -1)
			pthread_cancel(serverpid);
		delete sock;
	}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void process_messages();
	std::string ACTIVITY_VISIBLE GetName()
	{
		return "LocalWorldState";
	}

	void RobotPositionMotionModel(KMotionModel & MModel);
	void calculate_ball_estimate(KMotionModel const & MModel);
private:
	//check if the first odometry data had come
	bool firstOdometry;

	int serverpid;
	
	//WorldInfo message
	WorldInfo MyWorld;

	//Current agent position
	belief AgentPosition;

	//localization world
	KLocalization localizationWorld;	

	//Observations and odometry data to feed localization
	vector<KObservationModel> currentObservation;
	vector<KObservationModel> currentAmbiguousObservation;
	KMotionModel robotmovement;

	//Use a particle to store the odometry data
	partcl TrackPoint;
	partcl TrackPointRobotPosition;

	//Messages to feed the guis
	LocalizationData DebugData;
	LocalizationDataForGUI DebugDataForGUI;

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
	boost::posix_time::ptime timeStart,timeStop;
	boost::posix_time::ptime last_observation_time;
	boost::posix_time::ptime last_filter_time;
	boost::posix_time::ptime now;

	//Usefull for gui tools
	static void * StartServer(void * kati);
	pthread_t acceptthread;
	static TCPSocket *sock;
	int LocalizationData_Load(vector<KObservationModel> & Observation, KMotionModel & MotionModel);
	int LocalizationDataForGUI_Load();
	void Send_LocalizationData();
	static bool debugmode;
	header incommingheader;
	header outgoingheader;
	int size;
	char *data;
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*::  This function converts decimal degrees to radians             :*/
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	double deg2rad(double deg) {
		return (deg * M_PI / 180.0);
	}

	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	/*::  This function converts radians to decimal degrees             :*/
	/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
	double rad2deg(double rad) {
		return (rad * 180.0 / M_PI);
	}

};
ACTIVITY_END
#endif
