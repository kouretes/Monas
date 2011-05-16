
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
			
class TrCond_0_3_2_3_6_2TOMyLeftKick : public statechart_engine::ICondition {
			
public:

	 void UserInit () { 
		_blk->subscribeTo("vision", 0);
		_blk->subscribeTo("behavior", 0);
     }

	bool Eval() {
		Logger::Instance().WriteMsg("TrCond_0_3_2_3_6_2TOMyLeftKick", " ", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if (obsm.get()==0){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE NO OBSM", Logger::Info);
			return false;
		}else if (obsm.get()!=0 && (obsm->ball().dist() * sin(obsm->ball().bearing()) > -0.01) ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "TRUE", Logger::Info);
			return true;
		}else{
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE", Logger::Info);
			return false;
		}
    }
};
		
