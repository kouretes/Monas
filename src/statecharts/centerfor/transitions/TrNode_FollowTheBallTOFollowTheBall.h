
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_FollowTheBallTOFollowTheBall : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		 }

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(400000);
		_blk->process_messages();
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
     if ( gsm.get() == 0 ){
		 //std::cout<<"*************Transition NoPlay TRUE NO GSM*****************"<<std::endl;
		// Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " TRUE NO GSM", Logger::Info);
		 return false;
		 }
     if( gsm->player_state()!=PLAYER_PLAYING){
		//std::cout<<"*************Transition NoPlay FALSE*****************"<<std::endl;
		//Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " FALSE PLAYING", Logger::Info);
        return false;
	}
	 //std::cout<<"*************Transition NoPlay TRUE*****************"<<std::endl;
	// Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay",  " TRUE", Logger::Info);
	 return true;
    }
    
};
		
