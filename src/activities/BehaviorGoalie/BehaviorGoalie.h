#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "alptr.h"

#ifndef TO_RAD
#define TO_RAD 0.01745329f
#endif

#define LIMITUP -0.40
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.30
#define	LIMITRIGHT -0.30
#define STEPVER 0.35
#define STEPHOR 0.35

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class BehaviorGoalie: public IActivity, public Publisher {

	  public:
		BehaviorGoalie();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
		void CalculateBallSpeed();
		std::string GetName() {
			return "BehaviorGoalie";
		}

	private:

		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		short ballfound;
		MotionWalkMessage* wmot;
		MotionHeadMessage* hmot;
		MotionActionMessage* amot;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;
		BallTrackMessage lastballseen;

		bool startscan;
		bool scanforball;
		short scandirectionpitch;
		short scandirectionyaw;

		bool reachedlimitup;
		bool reachedlimitdown;
		bool reachedlimitleft;
		bool reachedlimitright;

		short balllastseendirection;
		HeadJointSensorsMessage* hjsm;
		BallTrackMessage* bmsg;
		GameStateMessage* gsm;
		ObservationMessage* obsm;
		int calibrated;
		bool play;

		bool stopped;
		bool readytokick;
		int back;
		int direction;
		bool turning;
		int count;
		int gameState;
		
		double mglRand();
		void velocityWalk(double x, double y, double th, double f);
		void littleWalk(double x, double y, double th, int s);
		void calibrate();

		double ballSpeedX;
		double ballSpeedY;
		bool speedIsValid;
		boost::posix_time::ptime prevTimestamp;
		double prevX;
		double prevY;
		
		double prevbd;
		double prevbb;
		
		int go; 
};

#endif
