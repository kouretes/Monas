#ifndef LOCALWORLDSTATE_H
#define LOCALWORLDSTATE_H

#include <string>
#include <pthread.h>
#include <netinet/in.h>
#include <math.h>
#include <csignal>

#include "core/include/IActivity.hpp"
#include "core/include/Configurator.hpp"

//#include "hal/robot/nao/generic_nao/robot_consts.h"

#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/Motion.pb.h"
#include "messages/Debug.pb.h"

#include "BallFilter.h"
#include "KLocalization.h"
#include "EKFLocalization.h"
#include "LocalizationStructs.h"

ACTIVITY_START
class LocalWorldState: public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(LocalWorldState)
	ACTIVITY_VISIBLE ~LocalWorldState()
	{

	}
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();
	void ProcessMessages();

	std::string ACTIVITY_VISIBLE GetName()
	{
		return "LocalWorldState";
	}

	void RobotPositionMotionModel(Localization::KMotionModel & MModel);
	void calculateBallEstimate(Localization::KMotionModel const & MModel);

private:
	//check if the first odometry data had come
	bool firstOdometry;

    //read xml files
    void ReadLocConf();
    void ReadFieldConf();
    void ReadFeatureConf();
    void ReadTeamConf();
    void ReadRobotConf();

	//WorldInfo message
	WorldInfo MyWorld;
    OdometryInfoMessage odometryInfoM;
    EKFMHypothesis ekfMHypothesis;
	//Current agent position
	Localization::belief AgentPosition;

	//localization world
	KLocalization localizationWorld;
    EKFLocalization ekfLocalization;

	//Observations and odometry data to feed localization
	vector<Localization::KObservationModel> currentObservation;
	vector<Localization::KObservationModel> currentAmbiguousObservation;
	Localization::KMotionModel robotmovement;

	//Use a particle to store the odometry data
	Localization::partcl TrackPoint;
	Localization::partcl TrackPointRobotPosition;

	//Messages to feed the guis
	LocalizationData DebugData;
	LocalizationDataForGUI DebugDataForGUI;

	//Store the odometry data from the sensors message
	SensorData PosX;
	SensorData PosY;
	SensorData Angle;

	//Our ball filter
	BallFilter myBall;

    int gameState,prevGameState;
	//Message we need to read
	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const RobotPositionMessage> rpsm;
	boost::shared_ptr<const LocalizationResetMessage> lrm;
	boost::shared_ptr<const MotionStateMessage> sm;
	MotionStateMessage::ActionType currentRobotAction;

	//Check if fall of the robot just began
	bool fallBegan;

	//Logger for all input and outputs for every execute cycle
	void InputOutputLogger();

	//Time variables
	KSystem::Time::TimeAbsolute timeStart, timeStop;
	KSystem::Time::TimeAbsolute lastObservationTime, gamePlaying;
	KSystem::Time::TimeAbsolute lastFilterTime;
	KSystem::Time::TimeAbsolute now;
    KSystem::Time::TimeAbsolute odometryMessageTime;
    KSystem::Time::TimeAbsolute debugMessageTime;
	//Usefull for gui tools

	int LocalizationDataForGUILoad();

    bool actionKick;
	bool gameMode;
	unsigned int stability;
    int ballTimeReset;
    // struct to store xml data
    Localization::LocConfig locConfig;

};
ACTIVITY_END
#endif
