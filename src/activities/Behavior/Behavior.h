#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "alptr.h"

#define LIMITUP -0.22
#define	LIMITDOWN 0.50
#define	LIMITLEFT 0.60
#define	LIMITRIGHT -0.60

namespace AL {
	class ALMotionProxy;
	class ALMemoryProxy;
}

class Behavior: public IActivity, public Publisher {

	public:
		Behavior();
		int Execute();
		void UserInit();
		void process_messages();

	private:
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;
		short ballfound;
		MotionMessage* mot;

		int pitchdirection;
		int yawdirection;
		SensorPair HeadYaw;
		SensorPair HeadPitch;

		bool startscan;
		bool scanningforball;
		short scandirectionpitch;
		short scandirectionyaw;

		bool reachedlimitup;
		bool reachedlimitdown;
		bool reachedlimitleft;
		bool reachedlimitright;

		short balllastseendirection;


};

#endif  
