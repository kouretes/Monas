
#ifndef _GoToPosition_h_
#define _GoToPosition_h_ 1

#include "architecture/IActivity.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/motion.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "../BehaviorConst.h"
#include "../ApproachBall/ApproachBall.h"
	
		
class GoToPosition : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
	boost::shared_ptr<const PositionMessage> pm;
	boost::shared_ptr<const WorldInfo> wimsg;
	BToHeadMessage* bhmsg;
	MotionWalkMessage wmot;
	float posX, posY, theta;
	float myPosX, myPosY, myPhi;
	float relativeX, relativeY, relativePhi;
	float dist;
	int side;
	int headaction;
	ApproachBall ab;
		
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	

};

#endif // _GoToPosition_h_
	
