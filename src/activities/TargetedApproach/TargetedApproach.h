
#ifndef _TargetedApproach_h_
#define _TargetedApproach_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include "../BehaviorConst.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"	
#include "../ApproachBall/ApproachBall.h"	
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <math.h>		

class TargetedApproach : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:	
	ApproachBall ab;
	MotionWalkMessage wmot;	
	BToHeadMessage* bhmsg;
	WorldInfo* lastObs;
	boost::posix_time::ptime rcvObs;
	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	int headaction;
	float postX, postY, theta;
	float ballX, ballY, myPosX, myPosY, myPhi;
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	
	float calculateAngle( float bPosX, float bPosY, float rPosX, float rPosY, float rPhi );
};

#endif // _AproachBall_h_
	
