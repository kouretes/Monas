#ifndef HeadBehavior_H
#define HeadBehavior_H

#include "architecture/executables/IActivity.h"

#include "tools/BehaviorConst.h"
#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Kimage.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "hal/robot/generic_nao/robot_consts.h"
#include <boost/date_time/posix_time/posix_time.hpp>




#define PITCHMIN -0.55
#define	PITCHMAX 0.33
#define YAWMIN 0.8
#define YAWMAX 1.35
#define PITCHSTEP 0.1
#define YAWSTEP 0.4
#define YAWSTEP1 0.35
#define PITCH1 -0.16
#define PITCH2 -0.55
#define PITCH3 -0.67

#define YAW1 -0.86
#define YAW2 0.86
#define YAW3 0.66
#define YAW4 0.0
#define YAW5 -0.66

#define OVERSH 0.06
#define WAITFOR 40


ACTIVITY_START
class HeadBehavior: public IActivity {

	public:
        ACTIVITY_CONSTRUCTOR(HeadBehavior);
		int ACTIVITY_VISIBLE IEX_DIRECTIVE_HOT Execute();
		void ACTIVITY_VISIBLE UserInit();

		std::string ACTIVITY_VISIBLE GetName() {
			return "HeadBehavior";
		}

	private:

		MotionHeadMessage* hmot;
		HeadToBMessage* hbmsg;
		ScanMessage* scmsg;
		SensorData HeadYaw;
		SensorData HeadPitch;

		int headaction;
		int prevaction;
		int curraction;
		int state;
		float headpos;
		int leftright;
		bool startscan;
		float targetYaw;
		float targetPitch;
		float psign,ysign;
		unsigned waiting;
		float obsmbearing,lastbearing;
		bool newBearing;

		boost::shared_ptr<const BallTrackMessage> bmsg,lastgoodbmsg;
		boost::shared_ptr<const BToHeadMessage> bhm;
		boost::shared_ptr<const AllSensorValuesMessage> asvm;
		boost::shared_ptr<const ObservationMessage> obsm;
		int calibrated;
		int step;
		boost::posix_time::ptime ballLastSeen,ballFirstSeen;
		boost::posix_time::ptime GoalLastSeen,GoalFirstSeen;

		void read_messages();
		void calibrate();
		void HeadScanStep();
		void highheadscanstep(float limit_yaw);
		void HeadScanStepSmart();
		void HeadScanStepFieldUntested();
		int MakeTrackBallAction();
		void headmotion(float pitch, float yaw);

};
ACTIVITY_END

#endif
