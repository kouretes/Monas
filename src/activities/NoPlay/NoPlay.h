
#ifndef _NoPlay_h_
#define _NoPlay_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"

#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4		
	
class NoPlay : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:

	bool kickOff;
	int curraction, prevaction;	
	boost::shared_ptr<const GameStateMessage> gsm;
	KickOffMessage* kcm;
	BToHeadMessage* bhmsg;
	MotionWalkMessage* wmot;
	int teamColor;
	int playernum;
	void velocityWalk(double x, double y, double th, double f);	
};

#endif // _NoPlay_h_
	
