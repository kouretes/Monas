
#ifndef _SitDownAndStare_h_
#define _SitDownAndStare_h_ 1

#include "architecture/IActivity.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"		
#include "tools/BehaviorConst.h"


class SitDownAndStare : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
	BToHeadMessage* bhmsg;
	MotionActionMessage* amot;
	boost::shared_ptr<const HeadToBMessage> hbm;
	boost::posix_time::ptime lastMove;
	boost::shared_ptr<const MotionStateMessage> msm;
	int headaction;

};

#endif // _SitDownAndStare_h_
	
