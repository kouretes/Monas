
#ifndef _SitDownAndStare_h_
#define _SitDownAndStare_h_ 1

#include "architecture/IActivity.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/motion.pb.h"
#include "messages/BehaviorMessages.pb.h"		
#include "tools/BehaviorConst.h"
	
#include "hal/robot/generic_nao/aldebaran-sensors.h"	
#include <alvalue/alvalue.h>
#include <alcore/alptr.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <alproxies/alrobotposeproxy.h>
#include "hal/robot/generic_nao/kAlBroker.h"

class SitDownAndStare : public IActivity {
			
public:
	
	int Execute ();
	
	void UserInit ();
	
	std::string GetName ();
	
private:
	BToHeadMessage* bhmsg;
	MotionActionMessage* amot;
	RobotStandingPose* rsp_msg;
	boost::shared_ptr<const HeadToBMessage> hbm;
	boost::shared_ptr<const RobotStandingPose> rsp;
	int headaction;
	AL::ALPtr<AL::ALRobotPoseProxy> robot;
	AL::ALPtr<AL::ALBroker> pbroker;
	AL::ALValue robotpose;
	std::string pose;
};

#endif // _SitDownAndStare_h_
	
