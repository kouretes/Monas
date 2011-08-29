#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "architecture/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"

#include <string>

#include "ISpecialAction.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif
#define NUM_OF_ANGLES 6
#define POSES_FORWKICK 5
#define POSES_BACKKICK 3
#define POSES_SIDEKICK 3

using namespace boost::posix_time;

//#define WEBOTS

class MotionController : public IActivity{

public:

	MotionController();

	int Execute();

	void UserInit();

	std::string GetName() {
		return "MotionController";
	}


private:

	AL::ALPtr<AL::DCMProxy> dcm;
	AL::ALPtr<AL::ALProxy> temp;
	AL::ALPtr<AL::ALMotionProxy> motion;
	AL::ALPtr<AL::ALBroker> pbroker;
	AL::ALPtr<AL::ALFrameManagerProxy> framemanager;

	bool walkingWithVelocity;

	bool robotDown;
	bool robotUp;
	//SensorPair AccZ, AccX, AccY;
	float AccZvalue, AccXvalue, AccYvalue;
	float accnorm, angX, angY, VangX, VangY;
	ptime waitfor;
	int gameState;
	int currentstate;

	int counter;

	int walkPID;
	float walkParam1, walkParam2, walkParam3, walkParam4;

	int headPID;
	float headParam1, headParam2;

	int actionPID;

	AL::ALValue names, values;

	boost::shared_ptr<const  MotionWalkMessage> wm;
	boost::shared_ptr<const  MotionHeadMessage> hm;
	boost::shared_ptr<const  MotionActionMessage> am;

	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const GameStateMessage>  gsm;

	MotionActionMessage  * mam;
	MotionActionMessage  * pam;

	SensorData LHipRoll;
	SensorData RHipPitch;
	SensorData LHipPitch;
	SensorData RHipRoll;
	SensorData RKneePitch;
	SensorData LKneePitch;

	AL::ALValue commands;//,stiffnessCommand;

	void testcommands();
	void mglrun();
	void read_messages();

	void stopWalkCommand();
	void killWalkCommand();
	void killHeadCommand();
	void killActionCommand();
	void killCommands();

	void ALstandUp();
	void ALstandUpCross();

	void ALstandUpBack();
	void ALstandUpFront();

	void MotionSkillsInit();
	int SpCutActionsManager();
	void AngleCompare(int numOfPoses);
	void readRobotLegConfiguration(const std::string& file_name);
	int motionSkills;
	int comp [3*(POSES_FORWKICK - 1)];
	float walkPrevAng [NUM_OF_ANGLES];

	float RKickAng [POSES_FORWKICK][NUM_OF_ANGLES];
	float LKickAng [POSES_FORWKICK][NUM_OF_ANGLES];
	int diffRKick [POSES_FORWKICK - 1][NUM_OF_ANGLES];
	int diffLKick [POSES_FORWKICK - 1][NUM_OF_ANGLES];
	float RBackKickAng [POSES_BACKKICK][NUM_OF_ANGLES];
	float LBackKickAng [POSES_BACKKICK][NUM_OF_ANGLES];
	int diffRBackKick [POSES_BACKKICK - 1][NUM_OF_ANGLES];
	int diffLBackKick [POSES_BACKKICK - 1][NUM_OF_ANGLES];

	float RSideKickAng [POSES_SIDEKICK][NUM_OF_ANGLES];
	float LSideKickAng [POSES_SIDEKICK][NUM_OF_ANGLES];
	int diffRSideKick [POSES_SIDEKICK - 1][NUM_OF_ANGLES];
	int diffLSideKick [POSES_SIDEKICK - 1][NUM_OF_ANGLES];

	void createDCMAlias();
	//void setStiffnessDCM(float s);
	typedef std::map<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;

	void readWalkParameters();

};

#endif
