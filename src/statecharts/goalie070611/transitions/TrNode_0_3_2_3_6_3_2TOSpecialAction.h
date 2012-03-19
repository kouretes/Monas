
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"

class TrCond_0_3_2_3_6_3_2TOSpecialAction : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* PLAYER_PLAYING and fall */
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2_3_2TOSpecialAction",  " enter", Logger::Info);

		boost::shared_ptr<const FallMessage> fm = _blk->readState<FallMessage>("behavior");
		int fall;
		if(fm.get()!=0)
			fall = fm->fall();
		else
			fall =0;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");

		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING ){

			if(fall==0)
				return false;
			else
				return true;
		}
		return false;
    }
};

