
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "tools/BehaviorConst.h"

class TrCond_ApproachBallTOApproachBall : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* PLAYER_PLAYING and !readyToKick and !ballAway and ballfound!=0 */
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
	//	Logger::Instance().WriteMsg("TrCond_ApproachBallTOApproachBall", " ", Logger::Info);
		//boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg  = _blk->readData<WorldInfo>("behavior");


		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(wimsg.get()!=0 && wimsg->balls_size()!=0 && !readyToKick(wimsg) && !ballAway(wimsg) )
				return true;
		}
		return false;
    }
};

