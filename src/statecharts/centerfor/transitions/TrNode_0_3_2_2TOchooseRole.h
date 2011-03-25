
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_0_3_2_2TOchooseRole : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		_blk->process_messages();
		 boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
     if ( gsm == 0 ){
		// Logger::Instance().WriteMsg("TrCond_0_3_2_2TOchooseRole",  " FALSE NO GSM", Logger::Info);
		 return false;
		 }
     if( gsm->player_state()!=PLAYER_PLAYING){
		//Logger::Instance().WriteMsg("TrCond_0_3_2_2TOchooseRole",  " FALSE", Logger::Info);
        return false;
	}
	// Logger::Instance().WriteMsg("TrCond_0_3_2_2TOchooseRole",  " TRUE", Logger::Info);
	 return true;
    }
};
		
