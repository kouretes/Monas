
#ifndef _Scan_h_
#define _Scan_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif	

#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4					
class Scan : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:	
		MotionWalkMessage* wmot;
		BToHeadMessage* bhmsg;
		boost::shared_ptr<const HeadToBMessage> hbm;
		boost::shared_ptr<const GameStateMessage> gsm;
		int headaction;
		boost::posix_time::ptime lastTurn;
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);	
	
};

#endif // _Scan_h_
	
