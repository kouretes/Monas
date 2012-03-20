
#ifndef _NoPlay_h_
#define _NoPlay_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/BehaviorConst.h"
#include "tools/MathFunctions.h"
	

class NoPlay : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
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
	bool cal;
	int teamColor;
	int playernum;
	float posX;
	float posY;
	float initX[2][2], initY[2][2], initPhi[2][2];

	
	int prevstate, currstate;
	
	bool readConf;
	std::string roleFName;
	void velocityWalk(double x, double y, double th, double f);
	void littleWalk(double x, double y, double th) ;
	void goToPosition(float x, float y, float phi);
	float rotation(float a, float b, float theta);
};

#endif // _NoPlay_h_
	
