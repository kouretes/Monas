
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
#include "../BehaviorConst.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <math.h>	
			
class ApproachBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	

	bool readyToKick( boost::shared_ptr<const ObservationMessage> msg1 );
	bool readyToKick( boost::shared_ptr<const WorldInfo> msg2 );
	bool ballAway( boost::shared_ptr<const ObservationMessage> msg1 );
	bool ballAway( boost::shared_ptr<const WorldInfo> msg2 );
	float distance(float x1, float x2, float y1, float y2);
	float rotation(float x, float y, float theta);

private:	
	MotionWalkMessage wmot;	
	BToHeadMessage* bhmsg;
	ObservationMessage* lastObsm;
	boost::posix_time::ptime rcvObsm;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const GameStateMessage> gsm;
	int headaction;
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	

};

#endif // _AproachBall_h_
	
