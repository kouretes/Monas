
#ifndef _FollowTheBall_h_
#define _FollowTheBall_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "../BehaviorConst.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif	

			
class FollowTheBall : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:	
		bool first;
		bool scancompleted;
		MotionWalkMessage* wmot;
		BToHeadMessage* bhmsg;
		ObservationMessage* lastObsm;
		boost::shared_ptr<const ObservationMessage> obsm;
		boost::shared_ptr<const ScanMessage> scm;
		boost::shared_ptr<const HeadToBMessage> hbm;
		boost::posix_time::ptime rcvObsm;
		int headaction;

		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);		
};

#endif // _FollowTheBall_h_
	
