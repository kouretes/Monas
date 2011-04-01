
#ifndef _Stare_h_
#define _Stare_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "../BehaviorConst.h"
#include "../ApproachBall/ApproachBall.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"	
			
class Stare : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	//float calculateBallSpeed(float x1, float y1, float, x2, float y2);
private:
	MotionWalkMessage wmot;	
	BToHeadMessage* bhmsg;
	ObservationMessage* lastObsm;
	boost::posix_time::ptime rcvObsm;
	boost::shared_ptr<const ObservationMessage> obsm;
	float observations[2][2];
	int headaction;
	void velocityWalk( double x, double y, double th, double f);
};

#endif // _Stare_h_
	
