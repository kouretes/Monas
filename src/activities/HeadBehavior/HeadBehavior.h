#ifndef HeadBehavior_H
#define HeadBehavior_H

#include "architecture/IActivity.h"
#include "architecture/narukom/pub_sub/publisher.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/BehaviorMessages.pb.h"

///#include "time.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include <boost/date_time/posix_time/posix_time.hpp>



#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define DONOTHING 0
#define CALIBRATE 1
#define SCANFORBALL 2
#define SCANFORPOST 3
#define BALLTRACK 4

#define PITCHMIN -0.55
#define	PITCHMAX 0.3
#define YAWMIN 0.8
#define YAWMAX 1.3
#define PITCHSTEP 0.22
#define YAWSTEP 0.45

#define OVERSH 0.06
#define WAITFOR 40

class HeadBehavior: public IActivity {

	public:
		HeadBehavior();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void MakeScanAction();
		void HeadScanStep();
		std::string GetName() {
			return "HeadBehavior";
		}

	private:

		MotionHeadMessage* hmot;
		HeadToBMessage* hbmsg;
		ScanMessage* scmsg;
		SensorData HeadYaw;
		SensorData HeadPitch;

		int headaction;
		boost::posix_time::ptime lastturn;

		bool startscan;
		bool scanforball;
		float targetYaw;
		float targetPitch;
		float psign,ysign;
		//bool pitchonly;
		unsigned waiting;
		float obsmbearing,lastbearing;
		bool newBearing;

		//boost::shared_ptr<const HeadJointSensorsMessage> hjsm;
		boost::shared_ptr<const BallTrackMessage> bmsg,lastgoodbmsg;

		boost::shared_ptr<const BToHeadMessage> bhm;
		boost::shared_ptr<const AllSensorValues> allsm;
		boost::shared_ptr<const ObservationMessage> obsm;
		int calibrated;

		boost::posix_time::ptime ballLastSeen,ballFirstSeen;
		boost::posix_time::ptime GoalLastSeen,GoalFirstSeen;

		void calibrate();

};

#endif
