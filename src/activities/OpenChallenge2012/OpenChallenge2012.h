#ifndef MOTION_CONTROLLER2_H
#define MOTION_CONTROLLER2_H

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/WorldInfo.pb.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"
#include "hal/robot/generic_nao/InverseKinematics.h"
#include "hal/robot/generic_nao/ForwardKinematics.h"

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
#include "ISpecialAction.h"
#include "KmexManager.h"
#include "KmexAction.h"
#include <boost/date_time/posix_time/posix_time.hpp>


#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

using namespace boost::posix_time;
using namespace std;
//#define WEBOTS

class OpenChallenge2012 : public IActivity{

public:

	OpenChallenge2012(Blackboard &b);;

	int Execute();

	void UserInit();

	std::string GetName() {
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
	boost::shared_ptr<const WorldInfo>  wim;
	
	boost::shared_ptr<const ObservationMessage>  obsm;
	SensorData LHYPitch,LHRoll,LHPitch,LKPitch,LARoll,LAPitch;
	SensorData RSPitch,RSRoll,REYaw,RERoll;
	float torsoHeight;
	bool donee;
	

	MotionActionMessage  * mam;
	MotionActionMessage  * pam;

	AL::ALValue commands;//,stiffnessCommand;

	void mglrun();
	void read_messages();

	void stopWalkCommand();
	void killWalkCommand();
	void killHeadCommand();
	void killActionCommand();
	void killCommands();

	vector<int> SpCutActionsManager();

	void createDCMAlias();
	//void setStiffnessDCM(float s);
	typedef std::map<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;


	typedef std::map<std::string,
		  boost::shared_ptr<KmexAction> > SpCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<KmexAction> > SpElement;
	SpCont SpKmexActions;
	boost::posix_time::ptime startl, stopl;
	boost::posix_time::ptime startr, stopr;
};

#endif
