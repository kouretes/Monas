
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "activities/Stare/Stare.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_2TOfollowBall_one_or_more_times_ : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		/* !readyToKick and ballfound!=0 and  (ballaway or (!ballaway and toFallOrNotToFall!=0) */
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOfollowBall_one_or_more_times_", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
	
		ApproachBall ab;
		Stare st;
		boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(obsm.get()!=0 ){
				if(!ab.readyToKick(obsm)){
					if( ab.ballAway(obsm)) //ball is far away from the goalpost
						return true;
					//ball is not far away from the goalpost but goalkeeper can repulse the ball	
					if(!ab.ballAway(obsm) && doi.get()!=0 && st.toFallOrNotToFall(doi)!=0 )
						return true;
				}
			}
		}
		return false;
	}
};
		