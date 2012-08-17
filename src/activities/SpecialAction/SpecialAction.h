
#ifndef _SpecialAction_h_
#define _SpecialAction_h_ 1

#include "architecture/executables/IActivity.h"
#include "activities/Stare/Stare.h"
#include "messages/BehaviorMessages.pb.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/BehaviorConst.h"


//ACTIVITY_START
class ACTIVITY_VISIBLE SpecialAction : public IActivity
{

public:

	ACTIVITY_CONSTRUCTOR(SpecialAction);
	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string ACTIVITY_VISIBLE GetName ();
	MotionActionMessage* amot;
	BToHeadMessage* bhm;
	ReturnToPositionMessage* rpm;
	boost::shared_ptr<const FallMessage> fm;
};
//ACTIVITY_END
#endif // _SpecialAction_h_

