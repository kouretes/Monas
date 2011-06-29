
#ifndef _MyLeftKick_h_
#define _MyLeftKick_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"		
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"

class MyLeftKick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	private:
		ReturnToPositionMessage* rpm;
		MotionActionMessage* amot;	
		boost::shared_ptr<const GameStateMessage> gsm;		
};

#endif // _MyLeftKick_h_
	
