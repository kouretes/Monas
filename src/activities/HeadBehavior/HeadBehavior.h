#ifndef HeadBehavior_H
#define HeadBehavior_H

#include "architecture/IActivity.h"
#include "architecture/archConfig.h"
#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/HeadToBMessage.pb.h"
#include "messages/BToHeadMessage.pb.h"

#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.63
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.30
#define	LIMITRIGHT -0.30
#define STEPVER 0.35
#define STEPHOR 0.35

#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4

class HeadBehavior: public IActivity {

	public:
		HeadBehavior();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
		std::string GetName() {
			return "HeadBehavior";
		}

	private:
		short ballfound;

		MotionHeadMessage* hmot;
		HeadToBMessage* hbmsg;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;

		int headaction;
		bool scancompleted;
		bool headstartscan;
		short scandirectionpitch;
		short scandirectionyaw;

		bool reachedlimitup;
		bool reachedlimitdown;
		bool reachedlimitleft;
		bool reachedlimitright;

		boost::shared_ptr<const HeadJointSensorsMessage> hjsm;
		boost::shared_ptr<const BallTrackMessage> bmsg;
		boost::shared_ptr<const BToHeadMessage> bhm;
		
		int calibrated;
		
		void calibrate();

};

#endif
