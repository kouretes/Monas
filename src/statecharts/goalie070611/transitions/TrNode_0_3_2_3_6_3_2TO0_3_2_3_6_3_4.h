
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
class TrCond_0_3_2_3_6_3_2TO0_3_2_3_6_3_4 : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* !PLAYER_PLAYING || ballfound==0 || !fall */

		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");

		boost::shared_ptr<const FallMessage> fm = _blk->readSignal<FallMessage>("behavior");
		int fall;
		//fall = st.toFallOrNotToFall(doi);
		if(fm.get()!=0)
			fall = fm->fall();
		else
			fall =0;

		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return true;
		return (fall==0);
    }
};

