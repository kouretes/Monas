
#ifndef _StandUpAndScan_h_
#define _StandUpAndScan_h_ 1

#include "architecture/executables/IActivity.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/BehaviorConst.h"


ACTIVITY_START

class StandUpAndScan : public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(StandUpAndScan);
	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();
private:
	BToHeadMessage* bhmsg;
	MotionActionMessage* amot;
	boost::shared_ptr<const HeadToBMessage> hbm;
	boost::shared_ptr<const MotionStateMessage> msm;
	int headaction;

	boost::posix_time::ptime lastMove;
};

ACTIVITY_END
#endif // _StandUpAndScan_h_

