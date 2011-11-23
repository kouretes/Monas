
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
class TrCond_0_3_2_2TOplay : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		 }

	bool Eval() {
		/* PLAYER_PLAYING */
			Logger::Instance().WriteMsg("TrCond_0_3_2_2TOplay", " enter  ", Logger::Info);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING)
			return true;
		return false;
    }
};

