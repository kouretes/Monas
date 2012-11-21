#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include <boost/date_time/posix_time/posix_time.hpp>

#include "architecture/executables/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupGameControlData.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"

#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/mathcommon.h"

#include "ISpecialAction.h"
#include "KmexManager.h"
#include "KmexAction.h"

using namespace boost::posix_time;
using namespace std;

ACTIVITY_START
class MotionController : public IActivity
{

public:

	MotionController(Blackboard &b, XmlManager &x);

	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
	void ACTIVITY_VISIBLE UserInit();
	void ACTIVITY_VISIBLE Reset();

	std::string ACTIVITY_VISIBLE GetName()
	{
		return "MotionController";
	}


private:

	AL::DCMProxy *dcm;
	AL::ALMotionProxy *motion;
	AL::ALFrameManagerProxy *framemanager;
	boost::shared_ptr<AL::ALProxy> temp;

	bool walkingWithVelocity;

	bool robotDown;
	bool robotUp;

	float AccZvalue, AccXvalue, AccYvalue;
	float accnorm, angX, angY, VangX, VangY;
	ptime waitfor;
	int gameState;
	int currentstate;

	int walkPID;
	float walkParam1, walkParam2, walkParam3, walkParam4;

	int headPID;
	float headParam1, headParam2;

	int actionPID;

	AL::ALValue names, values;
	AL::ALValue walkConfig;

	boost::shared_ptr<const  MotionWalkMessage> wm;
	boost::shared_ptr<const  MotionHeadMessage> hm;
	boost::shared_ptr<const  MotionActionMessage> am;

	boost::shared_ptr<const AllSensorValuesMessage> allsm;
	boost::shared_ptr<const GameStateMessage>  gsm;

	MotionActionMessage  *pam;
	MotionStateMessage	sm;
	AL::ALValue commands;

	void testcommands();
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


	typedef std::map < std::string, boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair < std::string, boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;

	typedef std::map < std::string, boost::shared_ptr<KmexAction> > SpCont;
	typedef std::pair < std::string, boost::shared_ptr<KmexAction> > SpElement;
	SpCont SpKmexActions;

	void readWalkParameters();
	boost::posix_time::ptime standUpStartTime, timeLapsed;
};

ACTIVITY_END

#endif
