
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/ApproachBall/ApproachBall.h"

class TrCond_StareTOStare : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();

		/* ballAway and ballfound!=0 and !fall */
	//	Logger::Instance().WriteMsg("TrCond_StareTOStare", "  ", Logger::Info);
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg  = _blk->readData<WorldInfo>("behavior");
		boost::shared_ptr<const FallMessage> fm  = _blk->readSignal<FallMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm  = _blk->readState<GameStateMessage>("behavior");
		ApproachBall ab;

		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//	Logger::Instance().WriteMsg("TrCond_StareTOStare", " !PLAYING false ", Logger::Info);
			return false;
		}

		if(hbm.get()!=0 && hbm->ballfound()!=0){
	//		Logger::Instance().WriteMsg("TrCond_StareTOStare", " ballfound==0 true ", Logger::Info);
			if(wimsg.get()!=0  && wimsg->balls_size()!=0 ){
				if( ab.ballAway(wimsg)) {//ball is far away from the goalpost
				//	Logger::Instance().WriteMsg("TrCond_StareTOStare", " !ballAway true ", Logger::Info);
					if(fm.get()!=0 && fm->fall()==0){
					//	Logger::Instance().WriteMsg("TrCond_StareTOStare", " fall true ", Logger::Info);
						return true;
					}
				}
			}
		}
		return false;
    }
};

