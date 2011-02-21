
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_FollowTheBallTO0_3_2_3_3_5_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		 }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
     if ( gsm.get() == 0 ){		
		// Logger::Instance().WriteMsg("TrCond_FollowTheBallTO0_3_2_3_3_5_3",  " TRUE NO GSM", Logger::Info);
		 return true;
		 }
     if( gsm->player_state()==PLAYER_PLAYING){
		//Logger::Instance().WriteMsg("TrCond_FollowTheBallTO0_3_2_3_3_5_3",  " FALSE PLAYING", Logger::Info);
        return false;
		}
	// Logger::Instance().WriteMsg("TrCond_FollowTheBallTO0_3_2_3_3_5_3",  " TRUE", Logger::Info);
	 return true;
    }
};
		
