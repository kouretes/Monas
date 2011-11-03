
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"


class TrCond_0_3_2_3_6_2TOMyRightKick : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
     }

	bool Eval() {
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_6_2TOMyRightKick", "FALSE NO OBSM", Logger::Info);
		//boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo> ("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if (wimsg.get()==0 || wimsg->balls_size()==0){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE NO OBSM", Logger::Info);
			return false;
		}else if ( wimsg->balls(0).relativey() <= -0.01 ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "TRUE", Logger::Info);
			return true;
		}else{
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE", Logger::Info);
			return false;
		}
    }
};

