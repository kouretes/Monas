#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"


#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "alptr.h"
#include "alvalue.h"
//#define WEBOTS
#include "alxplatform.h"
#include <albroker.h>
#include <alproxy.h>
#include <almemoryfastaccess.h>
#include "almotionproxy.h"
#include "almemoryproxy.h"
#include "albrokermanager.h"
#include "altexttospeechproxy.h"


class MotionController : public IActivity, public Publisher{

public:

	MotionController();

	int Execute();

	void UserInit();

	std::string GetName() {
		return "MotionController";
	}


private:

	AL::ALPtr<AL::ALProxy> tts;
	AL::ALPtr<AL::ALMotionProxy> motion;

	bool robotDown;
	bool robotUp;
	SensorPair AccZ;
	SensorPair AccX;
	float AccZvalue;
	float AccXvalue;

	int counter;

	int walkPID;
	float walkParam1, walkParam2, walkParam3, walkParam4;

	int headPID;
	float headParam1, headParam2;

	int actionPID;
	
	AL::ALValue names, values;

	MotionWalkMessage* wm;
	MotionHeadMessage* hm;
	MotionActionMessage* am;
	InertialSensorsMessage* im;

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
	
	void loadActions();
	AL::ALValue LieDown_names, LieDown_times, LieDown_keys;
	AL::ALValue LeftKick_names, LeftKick_times, LeftKick_keys;
	AL::ALValue RightKick_names, RightKick_times, RightKick_keys;

};

#endif
