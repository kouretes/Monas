
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
#include "../Localization/KLocalization.h"
#include <math.h>


using namespace boost::posix_time;

class ProtectPost : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();

private:	
	//ApproachBall ab;
	BToHeadMessage* bhmsg;
	WorldInfo* lastObs;
	//boost::shared_ptr<const WorldInfo> wimsg;
	boost::posix_time::ptime rcvObs, lastPostScan, lastMove;
	//boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	ReturnToPositionMessage* rpm;
	int headaction;
	MotionActionMessage* amot;
	MotionWalkMessage wmot;
	float myPosX, myPosY, myPhi;
	float estBallX, estBallY, targetx, targety;
	void blockBall();
	void GoToPosition(float x, float y, float theta);
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	
	
////////////////////////////////////////
	float distance(float x1, float x2, float y1, float y2);
	float rotation(float x, float y, float theta);
};

#endif // _ProtectPost_h_
	
