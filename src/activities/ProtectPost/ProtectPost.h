
#ifndef _ProtectPost_h_
#define _ProtectPost_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../BehaviorConst.h"
#include "../ApproachBall/ApproachBall.h"
#include <math.h>

class ProtectPost : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();

private:	
	ApproachBall ab;
	BToHeadMessage* bhmsg;
	ObservationMessage* lastObsm;
	boost::posix_time::ptime rcvObsm, lastPostScan;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	
	int headaction;
	MotionActionMessage* amot;
	MotionWalkMessage wmot;
	float myPosX, myPosY, myPhi;
	float estBallX, estBallY;
	void blockBall();
			
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	
};

#endif // _ProtectPost_h_
	
