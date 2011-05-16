
#ifndef _SpecialAction_h_
#define _SpecialAction_h_ 1

#include "architecture/IActivity.h"
#include "activities/Stare/Stare.h"			
#include "messages/BehaviorMessages.pb.h"
#include "tools/logger.h"
#include "tools/toString.h"		
#include "activities/BehaviorConst.h"
class SpecialAction : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	boost::shared_ptr<const DoubleObsInfo> obs;
	MotionActionMessage* amot;	
	BToHeadMessage* bhm;
};

#endif // _SpecialAction_h_
	
