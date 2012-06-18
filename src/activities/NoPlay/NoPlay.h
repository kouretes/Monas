
#ifndef _NoPlay_h_
#define _NoPlay_h_ 1

#include "architecture/executables/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/BehaviorConst.h"
#include "tools/MathFunctions.h"

ACTIVITY_START
class NoPlay : public IActivity {

public:
	ACTIVITY_CONSTRUCTOR(NoPlay);
	int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();
	bool readRobotConfiguration(const std::string& file_name);

private:

	bool kickOff;

	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const WorldInfo> wimsg;

	int curraction, prevaction;
	float myPosX, myPosY, myPhi;
	boost::posix_time::ptime lastMove, lastObsm, firstInit, calibrate_time;

	ReturnToPositionMessage* rpm;
	MotionWalkMessage wmot;
	MotionActionMessage amot;
	boost::shared_ptr<const MotionStateMessage> msm;
	KickOffMessage* kcm;
	BToHeadMessage* bhmsg;
	PositionMessage* pmsg;
	LocalizeDone* ld;
	LocalizationResetMessage* locReset;
	bool cal;
	int teamColor;
	int playernum;
	float posX;
	float posY;
	float initX[2], initY[2], initPhi[2];


	int prevstate, currstate;

	bool readConf;
	std::string roleFName;
	void velocityWalk(double x, double y, double th, double f);
	void littleWalk(double x, double y, double th) ;
	void goToPosition(float x, float y, float phi);
	float rotation(float a, float b, float theta);
};

ACTIVITY_END

#endif // _NoPlay_h_

