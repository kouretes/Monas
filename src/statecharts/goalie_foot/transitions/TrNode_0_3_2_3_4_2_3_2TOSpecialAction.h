
#include "architecture/statechartEngine/ICondition.h"
#include "activities/Stare/Stare.h"
#include "messages/AllMessagesHeader.h"
#include "tools/logger.h"
#include "tools/toString.h"

class TrCond_0_3_2_3_4_2_3_2TOSpecialAction : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* check ballspeed Fall */
	//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2_3_2TOSpecialAction",  " enter", Logger::Info);
		Stare st;
		boost::shared_ptr<const FallMessage> fm = _blk->readSignal<FallMessage>("behavior");
		int fall;
		//fall = st.toFallOrNotToFall(doi);
		if(fm.get()!=0)
			fall = fm->fall();
		else
			fall =0;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING){

			if(fall==0)
				return false;
			else
				return true;
		}
    }
};

