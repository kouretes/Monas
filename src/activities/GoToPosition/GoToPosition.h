
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
#include "../Localization/KLocalization.h"
#include "architecture/archConfig.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"

		
class GoToPosition : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
	bool robotInPosition(float rx, float x2, float ry, float y2, float rth, float th2);
	PositionMessage* pmsg;
private:
	boost::shared_ptr<const PositionMessage> pm;
	boost::shared_ptr<const WorldInfo> wimsg;
	boost::shared_ptr<const ObservationMessage> obsm;
	boost::shared_ptr<const GameStateMessage> gsm;
	int curr, prev;
	BToHeadMessage* bhmsg;
	MotionWalkMessage wmot;
	float posX, posY, theta;
	float myPosX, myPosY, myPhi;
	float relativeX, relativeY, relativePhi, confidence;
	float dist;
	int side;
	int headaction;
	ReturnToPositionMessage* rpm;
	boost::posix_time::ptime lastMove, lastObsm;
	void velocityWalk( double x, double y, double th, double f);
	void littleWalk(double x, double y, double th);	
	////////////////////////////////////////
	float distance(float x1, float x2, float y1, float y2);
	float rotation(float x, float y, float theta);

};

#endif // _GoToPosition_h_
	
