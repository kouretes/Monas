
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_0_3_2_2TONoPlay : public statechart_engine::ICondition {
			
public:

void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		_blk->process_messages();
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
     if ( gsm.get() == 0 ){		
		// Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " TRUE NO GSM", Logger::Info);
		 return true;
		 }
     if( gsm->player_state()==PLAYER_PLAYING){		
		//Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " FALSE PLAYING", Logger::Info);
        return false;
	}
	// Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " TRUE", Logger::Info);
	 return true;
    }
};
		
