
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"

class TrCond_0_3_2_4_3_2TOGoToPosition_one_or_more_times_ : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* PLAYER_READY */
	//	Logger::Instance().WriteMsg("TrCond_0_3_2_4_3_2TOGoToPosition_one_or_more_times_",  " enter", Logger::Info);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");
		return false;
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_READY)
			return true;
		return false;
    }
};

