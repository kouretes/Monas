
#ifndef _Kick_h_
#define _Kick_h_ 1

#include "architecture/IActivity.h"
#include "messages/AllMessagesHeader.h"
#include "tools/logger.h"
#include "tools/toString.h"
class Kick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
private:
	MotionActionMessage* amot;	
	boost::shared_ptr<const GameStateMessage> gsm;		
	boost::shared_ptr<const WorldInfo> wimsg;
};

#endif // _Kick_h_
	
