
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
			
class TrCond_decisionTO0_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* PLAYER_FINISHED */
	//	Logger::Instance().WriteMsg("TrCond_decisionTO0_3_3", "!FINISHED ",Logger::Info);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		
		//if(gsm.get()!=0 && gsm->player_state()==PLAYER_FINISHED)
		//	return true;
		return false;
    }
};
		
