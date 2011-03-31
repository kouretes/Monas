
#ifndef _KickOff_h_
#define _KickOff_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/timeout.pb.h"		
	
class KickOff : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
private:
	boost::shared_ptr<const GameStateMessage> gsm;
	MotionActionMessage* amot;
	TimeoutMsg* tmsg;
	boost::shared_ptr<const ObservationMessage> obsm;	
};

#endif // _KickOff_h_
	
