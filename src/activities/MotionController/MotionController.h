#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "architecture/IActivity.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "hal/robot/generic_nao/aldebaran-motion.h"

#include <string>

#include "ISpecialAction.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

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
	float accnorm, angX, angY, gyrX, gyrY;
	ptime waitfor;

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
	
	void AngleCompare();
	
	int comp[20];
	float RKickAng1 [6], RKickAng2 [6], RKickAng3 [6], RKickAng4 [6], RKickAng5 [6], RKickAng6 [6];
	float LKickAng1 [6], LKickAng2 [6], LKickAng3 [6], LKickAng4 [6], LKickAng5 [6], LKickAng6 [6];
	float walkPrevAng [6];
	int diffRKick1 [6], diffRKick2 [6], diffRKick3 [6], diffRKick4 [6];
	int diffLKick1 [6], diffLKick2 [6], diffLKick3 [6], diffLKick4 [6];
		
	void createDCMAlias();
	void setStiffnessDCM(float s);
	typedef std::map<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAssocCont;
	typedef std::pair<std::string,
		  boost::shared_ptr<ISpecialAction> > SpAsoocElement;
	SpAssocCont SpActions;

};

#endif
