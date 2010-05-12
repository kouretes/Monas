#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/Gamecontroller.pb.h"


#include "alptr.h"

#define LIMITUP -0.22
#define	LIMITDOWN 0.43
#define	LIMITLEFT 0.40
#define	LIMITRIGHT -0.40

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class Behavior: public IActivity, public Publisher {

	public:
		Behavior();
		int Execute();
		void UserInit();
		void read_messages();

        std::string GetName() {
            return "Behavior";
        }

	private:
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		short ballfound;
		MotionMessage* mot;

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
		bool calibrated;
		bool play;

		bool stopped;
};

#endif
