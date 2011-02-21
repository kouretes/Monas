
#ifndef _MyLeftSideKick_h_
#define _MyLeftSideKick_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"		
#include "messages/Gamecontroller.pb.h"				

			
class MyLeftSideKick : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
		MotionActionMessage* amot;	
		boost::shared_ptr<const GameStateMessage> gsm;			
};

#endif // _MyLeftSideKick_h_
	
