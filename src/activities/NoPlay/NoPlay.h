
#ifndef _NoPlay_h_
#define _NoPlay_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "../BehaviorConst.h"
#include "../ApproachBall/ApproachBall.h"
	

class NoPlay : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	bool readRobotConfiguration(const std::string& file_name, bool kickoff);	
	float initX,initY, initPhi;
	
private:
	ApproachBall ab;
	bool kickOff;
	int curraction, prevaction;	
	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const PlayerNumberMessage> pnm;
	ReturnToPositionMessage* rpm;
	MotionWalkMessage wmot;
	KickOffMessage* kcm;
	BToHeadMessage* bhmsg;
	PositionMessage* pmsg;
	bool cal;
	int teamColor;
	int playernum;
	float posX;
	float posY;
	bool readConf;
	std::string roleFName;
	void velocityWalk(double x, double y, double th, double f);
	void littleWalk(double x, double y, double th) ;

};

#endif // _NoPlay_h_
	