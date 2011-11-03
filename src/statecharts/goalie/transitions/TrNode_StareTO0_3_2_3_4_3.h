
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_StareTO0_3_2_3_4_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		/* ballfound==0 || ballaway */
		Logger::Instance().WriteMsg("TrCond_StareTO0_3_2_3_4_3", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");

		ApproachBall ab;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()==0)
			return true;
		if(obsm.get()!=0 && !ab.ballAway(obsm))
			return true;
		
		return false;
	}
};
