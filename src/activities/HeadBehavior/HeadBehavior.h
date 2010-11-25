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
///#include "time.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.55
#define	LIMITDOWN 0.19
#define	LIMITLEFT 0.5
#define	LIMITRIGHT -0.5
#define STEPVER 0.65
#define STEPHOR 0.2

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
		ScanMessage* scmsg;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;

		int headaction;
		int oldheadaction;
		bool choosemyaction;
		bool scancompleted;
		bool headstartscan;
		short scandirectionpitch;
		short scandirectionyaw;
		//bool vcalibrated;
		bool reachedlimitup;
		bool reachedlimitdown;
		bool reachedlimitleft;
		bool reachedlimitright;

		boost::shared_ptr<const HeadJointSensorsMessage> hjsm;
		boost::shared_ptr<const BallTrackMessage> bmsg;
		boost::shared_ptr<const BToHeadMessage> bhm;
		//boost::shared_ptr<const ObservationMessage> obsm;		
		int calibrated;
		//bool counttime;
		//time_t start;
		//time_t end;
		void calibrate();

};

#endif
