
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"	
			
class TrCond_WalktoPostTO_open_option_SearchBall_one_or_more_times__close_option_ : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
		 _blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		return true;
		
    }
};
		
