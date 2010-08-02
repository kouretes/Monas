#ifndef BEHAVIORPASSING_H
#define BEHAVIORPASSING_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/ObstacleAvoidanceMessage.pb.h"

#include "alptr.h"

#define LIMITUP -0.22
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.40
#define	LIMITRIGHT -0.40

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class BehaviorPassing: public IActivity, public Publisher {

	public:
		BehaviorPassing();
		int Execute();
		void UserInit();
		void read_messages();
		int MakeTrackBallAction();
		void HeadScanStep();
        std::string GetName() {
            return "BehaviorPassing";
        }
        void mgltest();

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
		ObservationMessage* obsm;
		GameStateMessage* gsm;
		ObstacleMessage* om;
		int calibrated;
		bool play;

		bool stopped;
		bool readytokick;
		int back;
		int direction;
		bool turning; 
		int count;
};

#endif
