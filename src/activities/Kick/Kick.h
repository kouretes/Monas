
#ifndef _Kick_h_
#define _Kick_h_ 1

#include "architecture/IActivity.h"
#include "messages/AllMessagesHeader.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "../BehaviorConst.h"
#include "../Localization/KLocalization.h"

class Kick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
private:
	KLocalization k;
	int orientation;
	float by;
	float blueGoalX, blueGoalY, yellowGoalX, yellowGoalY, oppGoalX, oppGoalY;
	MotionActionMessage* amot;	
	boost::shared_ptr<const GameStateMessage> gsm;		
	boost::shared_ptr<const WorldInfo> wimsg;
	boost::shared_ptr<const KickOffMessage> kcm;
	boost::shared_ptr<const PlayerNumberMessage> pnm;
	ReturnToPositionMessage* rpm;
	bool readConf;
	
	bool readGoalConfiguration(const std::string& file_name);
};

#endif // _Kick_h_
	
