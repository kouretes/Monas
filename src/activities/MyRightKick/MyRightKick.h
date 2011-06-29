
#ifndef _MyRightKick_h_
#define _MyRightKick_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/Gamecontroller.pb.h"	
#include "messages/BehaviorMessages.pb.h"

class MyRightKick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	private:
		ReturnToPositionMessage* rpm;
		MotionActionMessage* amot;	
		boost::shared_ptr<const GameStateMessage> gsm;	
};

#endif // _MyRightKick_h_
	
