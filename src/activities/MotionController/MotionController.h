#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "architecture/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"

#include <string>

#include "ISpecialAction.h"
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

	AL::ALPtr<AL::ALProxy> temp;
	AL::ALPtr<AL::ALMotionProxy> motion;
	AL::ALPtr<AL::ALBroker> pbroker;
	AL::ALPtr<AL::ALFrameManagerProxy> framemanager;

	bool walkingWithVelocity;

	bool robotDown;
	bool robotUp;
	//SensorPair AccZ, AccX, AccY;
	float AccZvalue, AccXvalue, AccYvalue;

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

	boost::shared_ptr<const AllSensorValues> allsm;


	void commands();
	void mglrun();
	void read_messages();

	void stopWalkCommand();
	void killWalkCommand();
	void killHeadCommand();
	void killActionCommand();
	void killCommands();

	void ALstandUp();
	void ALstandUpCross();
	void ALstandUpFront2009();
	void ALstandUpBack2009();
	void ALstandUpFront2010();
	void ALstandUpBack2010();

	typedef std::map<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;

};

#endif
