
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"	
#include "activities/ApproachBall/ApproachBall.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_2TOStare_one_or_more_times_ : public statechart_engine::ICondition {
			
public:
	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		/* ballfound!=0 and ballaway */
		Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOStare_one_or_more_times_", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");

		ApproachBall ab;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()==0)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(obsm.get()!=0 && ab.ballAway(obsm))
				return true;
		}
		return false;
	}
};
