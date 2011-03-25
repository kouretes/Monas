
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"
			
class TrCond_FollowTheBallTO0_3_2_3_3_4_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		}

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
     if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
		 
		Logger::Instance().WriteMsg("TrCond_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6",  " TRUE !PLAYING", Logger::Info);
		 return true;
		 }
     if( gsm->player_state()==PLAYER_PLAYING){	
		Logger::Instance().WriteMsg("TrCond_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6",  " FALSE PLAYING", Logger::Info);
        return false;
		}
	 Logger::Instance().WriteMsg("TrCond_FollowTheBall_one_or_more_times_TO0_3_2_3_3_6",  " TRUE", Logger::Info);
	 return true;
    }
};
		
