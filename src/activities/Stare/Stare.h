
#ifndef _Stare_h_
#define _Stare_h_ 1

#include "architecture/executables/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/WorldInfo.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time_duration.hpp>
#include "tools/BehaviorConst.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include <string.h>
#include <math.h>

ACTIVITY_START
class Stare : public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(Stare);

	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string  ACTIVITY_VISIBLE GetName ();
	//int toFallOrNotToFall(boost::shared_ptr<const DoubleObsInfo> doi);
	//	int toFallOrNotToFall(DoubleObsInfo* doi);
	int toFallOrNotToFall(boost::shared_ptr<const WorldInfo> doi);
private:
	MotionWalkMessage wmot;

	BToHeadMessage* bhmsg;
	FallMessage* fm;
	boost::shared_ptr<const WorldInfo> wimsg;
	int headaction;
	int fall;
	bool penaltyMode;
	void velocityWalk( double x, double y, double th, double f);
};

ACTIVITY_END

#endif // _Stare_h_

