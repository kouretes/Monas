
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "tools/BehaviorConst.h"

class TrCond_ScanTO0_3_2_3_3_3_3 : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* ballfound!=0 || return ToPosition */

		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");
		//boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");
		//boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//	Logger::Instance().WriteMsg("TrCond_ScanTO0_3_2_3_3_3_3", "!PLAYER_PLAYING ", Logger::Info);
			return true;
		}
		if(hbm.get()!=0 && hbm->ballfound()!=0){
		//	Logger::Instance().WriteMsg("TrCond_ScanTO0_3_2_3_3_3_3", "ballfound!=0 ", Logger::Info);
			return true;
		}

		//if(wimsg!=0 ){
			//Logger::Instance().WriteMsg("TrCond_ScanTO0_3_2_3_3_3_3", "Confidence " + _toString(wimsg->myposition().confidence()), Logger::Info);
			//if(wimsg->myposition().confidence()<badConfidence){
				//return false;
			//}else{
				//GoToPosition g;
				//if(pm!=0 && !g.robotInPosition(wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(), pm->posy(), wimsg->myposition().phi(), pm->theta() ))
					//return true;
			//}
		//}

		return false;
    }
};

