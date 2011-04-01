
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
			
class TrCond__open_group_decision_one_or_more_times__close_group_TO0_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* PLAYER_FINISHED */
		Logger::Instance().WriteMsg("decision forever", "!FINISHED ",Logger::Info);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_FINISHED)
			return true;
		return false;
    }
};
		
