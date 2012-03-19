
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "tools/BehaviorConst.h"

class TrCond_0_3_2_3_2TOfollowTheBall : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
		}

	bool Eval() {
		/* ballfound!=0 and ballaway */
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOfollowBall_one_or_more_times_", "  ", Logger::Info);
	//	boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg  = _blk->readData<WorldInfo>("worldstate");
		
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(wimsg.get()!=0  && wimsg->balls_size()!=0 ){
				if(!readyToKick(wimsg)){
					if( ballAway(wimsg)) //ball is far away from the goalpost
						return true;
					//ball is not far away from the goalpost but goalkeeper can repulse the ball
				}
			}
		}
		return false;
    }
};

