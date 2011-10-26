
#ifndef _Stare_h_
#define _Stare_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time_duration.hpp>
#include "../BehaviorConst.h"
#include "../ApproachBall/ApproachBall.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"	
#include <string.h>
#include <string.h>
#include <math.h>
class Stare : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	//int toFallOrNotToFall(boost::shared_ptr<const DoubleObsInfo> doi);
	int toFallOrNotToFall(DoubleObsInfo* doi);
	int toFallOrNotToFall(WorldInfo* doi);
private:
	ApproachBall ab;
	MotionWalkMessage wmot;	

	BToHeadMessage* bhmsg;
	FallMessage* fm;
	ObservationMessage* lastObsm;
	WorldInfo* lastWimsg;
	DoubleObsInfo* doim;
	boost::posix_time::ptime rcvObsm, rcvWimsg;
	boost::posix_time::ptime lastMove;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	int headaction;
	int fall;
	bool penaltyMode;
	void velocityWalk( double x, double y, double th, double f);
};

#endif // _Stare_h_
	