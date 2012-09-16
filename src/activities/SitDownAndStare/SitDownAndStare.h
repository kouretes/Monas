
#ifndef _SitDownAndStare_h_
#define _SitDownAndStare_h_ 1

#include "architecture/executables/IActivity.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/BehaviorConst.h"

ACTIVITY_START
class SitDownAndStare : public IActivity
{

public:
	ACTIVITY_CONSTRUCTOR(SitDownAndStare)

	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();

private:
	BToHeadMessage* bhmsg;
	MotionActionMessage* amot;
	boost::shared_ptr<const HeadToBMessage> hbm;
	boost::posix_time::ptime lastMove;
	boost::shared_ptr<const MotionStateMessage> msm;
	int headaction;

};
ACTIVITY_END
#endif // _SitDownAndStare_h_

