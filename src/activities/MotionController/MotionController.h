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

#include "architecture/narukom/pub_sub/filters/type_filter.h"
#include <string>

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
	TypeFilter* type_filter;
    
    bool walkingWithVelocity;

	bool robotDown;
	bool robotUp;
	SensorPair AccZ, AccX, AccY;
	float AccZvalue, AccXvalue, AccYvalue;

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
	std::vector<std::string> RightDive_names;
	AL::ALValue RightDive_times, RightDive_keys;
	std::vector<std::string> LeftDive_names;
	AL::ALValue LeftDive_times, LeftDive_keys;
	std::vector<std::string> PuntKick_names;
	AL::ALValue PuntKick_times, PuntKick_keys;
	std::vector<std::string> HardLeftSideKick_names;
	AL::ALValue HardLeftSideKick_times, HardLeftSideKick_keys;
	std::vector<std::string> SoftLeftSideKick_names;
	AL::ALValue SoftLeftSideKick_times, SoftLeftSideKick_keys;
	std::vector<std::string> HardRightSideKick_names;
	AL::ALValue HardRightSideKick_times, HardRightSideKick_keys;
	std::vector<std::string> SoftRightSideKick_names;
	AL::ALValue SoftRightSideKick_times, SoftRightSideKick_keys;
	std::vector<std::string> LeftBackKick_names;
	AL::ALValue LeftBackKick_times, LeftBackKick_keys;
	std::vector<std::string> RightBackKick_names;
	AL::ALValue RightBackKick_times, RightBackKick_keys;

	void loadActionsKME();
	void loadBackKicks();
	struct motSequence {
		std::string seqName;
		std::vector< std::vector<float> > seqMotion;
	};
	std::vector<motSequence> spAct;
	std::map<std::string, int> actionMap;
	void printActionsKME();
	
	std::vector<std::string> jointNames;
	int executeActionKME(std::string action);
	int executeActionBodyKME(std::string action);
};

#endif
