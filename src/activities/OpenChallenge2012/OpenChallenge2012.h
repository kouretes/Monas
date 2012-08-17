#ifndef OpenChallenge2012_H
#define OpenChallenge2012_H

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/VisionObservations.pb.h"


#include "hal/robot/generic_nao/aldebaran-motion.h"
#include "hal/robot/generic_nao/NAOKinematics.h"
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
#include "messages/RoboCupGameControlData.h"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ISpecialAction.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

using namespace boost::posix_time;
using namespace std;
//#define WEBOTS


ACTIVITY_START
class OpenChallenge2012 : public IActivity{

public:

	OpenChallenge2012(Blackboard &b,XmlNode &x);

	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();

	std::string ACTIVITY_VISIBLE GetName() {
		return "OpenChallenge2012";
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

	NAOKinematics *nkin;
	int walkPID;
	float walkParam1, walkParam2, walkParam3, walkParam4;

	int headPID;
	float headParam1, headParam2;

	int actionPID;
	int counter;

	AL::ALValue names, values;

	boost::shared_ptr<const  MotionWalkMessage> wm;
	boost::shared_ptr<const  MotionHeadMessage> hm;
	boost::shared_ptr<const  MotionActionMessage> am;

	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const GameStateMessage>  gsm;
	boost::shared_ptr<const WorldInfo>  wim;
	
	boost::shared_ptr<const ObservationMessage>  obsm;
	SensorData LHYPitch,LHRoll,LHPitch,LKPitch,LARoll,LAPitch;
	SensorData RSPitch,RSRoll,REYaw,RERoll;
	float torsoHeight;
	bool donee;
	

	MotionActionMessage  * pam;
	MotionStateMessage	sm;
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

	vector<float> KGetAngles();
	void createDCMAlias();
	//void setStiffnessDCM(float s);
	typedef std::map<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;


	std::string BodyID;
	void readWalkParameters();
	boost::posix_time::ptime standUpStartTime, timeLapsed;
	boost::posix_time::ptime startl, stopl;
	boost::posix_time::ptime startr, stopr;
};

ACTIVITY_END

#endif
