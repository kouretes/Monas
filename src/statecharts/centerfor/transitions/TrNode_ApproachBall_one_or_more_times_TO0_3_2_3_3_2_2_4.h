#include "messages/BehaviorMessages.pb.h"
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "messages/WorldInfo.pb.h"
#include "architecture/archConfig.h"		
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior",0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("behavior");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4", "TRUE NO GSM", Logger::Info);
			return true;
		}
		if (hbm.get()!=0 && hbm->ballfound()==0){
			Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4", "TRUE ballfound ==0", Logger::Info);
			return true;
		}
		if ( obsm.get() != 0 ){
			
			int side;
			float posx=0.117, posy=0.03;
			
			side = (obsm->ball().bearing() > 0) ? 1 : -1;
			
			if ( (fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.012 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <= 0.012) ){
				Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4", "TRUE", Logger::Info);
				return true;
			}else{
				Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_3_2_2_4", "FALSE", Logger::Info);
				return false;
			}
		}
		return false;
    }
};
		
