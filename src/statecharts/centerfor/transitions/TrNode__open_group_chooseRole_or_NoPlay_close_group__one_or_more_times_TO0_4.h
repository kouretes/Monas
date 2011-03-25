
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"		
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4 : public statechart_engine::ICondition {
			
public:

	
void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		//_blk->process_messages();
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
     
     if( gsm.get()!=0 && gsm->player_state()==PLAYER_FINISHED){		
	//	Logger::Instance().WriteMsg("TrCond__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4",  " FALSE FINISHED", Logger::Info);
        return true;
	}
//	 Logger::Instance().WriteMsg("TrCond__open_group_chooseRole_or_NoPlay_close_group__one_or_more_times_TO0_4",  " TRUE", Logger::Info);
	 return false;
    }
};
		
