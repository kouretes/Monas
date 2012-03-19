
#ifndef _ApproachBall_h_
#define _ApproachBall_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/BehaviorConst.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <math.h>	
			
class ApproachBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	

private:	
	MotionWalkMessage wmot;	
	BToHeadMessage* bhmsg;
	ObservationMessage* lastObsm;
	boost::posix_time::ptime rcvObsm;
	boost::posix_time::ptime lastMove;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	boost::shared_ptr<const GameStateMessage> gsm;
	ReturnToPositionMessage* rpm;
	int headaction;
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	


};

#endif // _AproachBall_h_
	
