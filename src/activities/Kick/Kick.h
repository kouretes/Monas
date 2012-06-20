
#ifndef _Kick_h_
#define _Kick_h_ 1

#include "architecture/executables/IActivity.h"
#include "messages/AllMessagesHeader.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "tools/BehaviorConst.h"
#include "tools/MathFunctions.h"


ACTIVITY_START
class Kick : public IActivity {

public:
    ACTIVITY_CONSTRUCTOR(Kick);

	int ACTIVITY_VISIBLE Execute ();

	void ACTIVITY_VISIBLE UserInit ();

	std::string  ACTIVITY_VISIBLE GetName ();
private:

	int orientation;
	float by;
	MotionActionMessage* amot;
	boost::shared_ptr<const GameStateMessage> gsm;
	boost::shared_ptr<const WorldInfo> wimsg;
	boost::shared_ptr<const KickOffMessage> kcm;
	ReturnToPositionMessage* rpm;
	bool readConf;
	double oppGoalX, oppGoalY, ownGoalX, ownGoalY;
	double oppGoalLeftX, oppGoalLeftY, oppGoalRightX, oppGoalRightY;
	double ownGoalLeftX, ownGoalLeftY, ownGoalRightX, ownGoalRightY;
	bool readGoalConfiguration(const std::string& file_name);
};

ACTIVITY_END

#endif // _Kick_h_

