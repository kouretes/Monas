
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "tools/BehaviorConst.h"

class TrCond_ApproachBallTO0_3_2_3_4_3 : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* !PLAYER_PLAYING || reayToKick || ballAway || ballfound==0 */
	//	Logger::Instance().WriteMsg("TrCond_ApproachBallTO0_3_2_3_5_3", "  ", Logger::Info);
	//	boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg  = _blk->readData<WorldInfo>("worldstate");

		//ReturnToPositionMessage* rpm = new ReturnToPositionMessage();
		//rpm->set_goalietopos(true);
		//_blk->publishSignal(*rpm, "behavior");

		
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()==0)
			return true;
		if(wimsg.get()!=0 && wimsg->balls_size()!=0 && readyToKick(wimsg))
				return true;
		if(wimsg.get()!=0 && wimsg->balls_size()!=0 && ballAway(wimsg))
			return true;
		return false;
    }
};

