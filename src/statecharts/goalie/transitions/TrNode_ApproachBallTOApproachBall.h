
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_ApproachBallTOApproachBall : public statechart_engine::ICondition {
			
public:

	/* ballfound!=0 and !readyToKick */
	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		Logger::Instance().WriteMsg("TrCond_ApproachBallTOApproachBall", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");

		ApproachBall ab;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(obsm.get()!=0 && !ab.readyToKick(obsm) && !ab.ballAway(obsm) )
				return true;
		}
		return false;
    }
};
