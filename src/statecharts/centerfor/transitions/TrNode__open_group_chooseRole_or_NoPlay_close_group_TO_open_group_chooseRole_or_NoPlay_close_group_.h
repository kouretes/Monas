
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"		
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond__open_group_chooseRole_or_NoPlay_close_group_TO_open_group_chooseRole_or_NoPlay_close_group_ : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0); 
		_blk->subscribeTo("vision", 0);
		}

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(100000);
		_blk->process_messages();
		/* true */
		return true;
    }
};
