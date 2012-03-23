
#ifndef _Scan_h_
#define _Scan_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "tools/BehaviorConst.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"			

class Scan : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:	
	BToHeadMessage* bhmsg;
	MotionActionMessage* amot;

	boost::shared_ptr<const HeadToBMessage> hbm;
	boost::shared_ptr<const MotionStateMessage> msm;

	int headaction;

	
};

#endif // _Scan_h_
	
