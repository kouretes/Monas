#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "architecture/IActivity.h"

#include "architecture/narukom/pub_sub/publisher.h"

#include "messages/motion.pb.h"

#include "alptr.h"
namespace AL {
    class ALMotionProxy;
    class ALMemoryProxy;
}

class Behavior : public IActivity, public Publisher {

	public:
		Behavior();
		int Execute();

        void UserInit();
        
		void process_messages();

	private:
		AL::ALPtr<AL::ALMotionProxy> motion;
		AL::ALPtr<AL::ALMemoryProxy> memory;

		MotionMessage* mot;
};

#endif  
