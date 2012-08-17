#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"

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

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

using namespace boost::posix_time;
using namespace std;
//#define WEBOTS


ACTIVITY_START
class MotionController : public IActivity
{

public:

	MotionController(Blackboard &b, XmlNode &x);

	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();

	std::string ACTIVITY_VISIBLE GetName()
	{
		return "MotionController";
	}


private:

	AL::DCMProxy *dcm;
	boost::shared_ptr<AL::ALProxy> temp;
	boost::shared_ptr<AL::ALMotionProxy> motion;
	boost::shared_ptr<AL::ALBroker> pbroker;
	boost::shared_ptr<AL::ALFrameManagerProxy> framemanager;

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

	vector<int> SpCutActionsManager();
	vector<float> KGetAngles();
	void createDCMAlias();
	//void setStiffnessDCM(float s);
	typedef std::map < std::string,
	        boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair < std::string,
	        boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;


	typedef std::map < std::string,
	        boost::shared_ptr<KmexAction> > SpCont;
	typedef std::pair < std::string,
	        boost::shared_ptr<KmexAction> > SpElement;
	SpCont SpKmexActions;

	std::string BodyID;
	void readWalkParameters();
	boost::posix_time::ptime standUpStartTime, timeLapsed;
};

ACTIVITY_END

#endif
