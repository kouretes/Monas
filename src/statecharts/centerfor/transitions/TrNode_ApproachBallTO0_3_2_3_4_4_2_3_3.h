
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"					
#include "tools/toString.h"
#include "tools/logger.h"
#include "messages/BehaviorMessages.pb.h"
			
class TrCond_ApproachBallTO0_3_2_3_4_4_2_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {

		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("behavior");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			Logger::Instance().WriteMsg("TrCond_ApproachBallTO0_3_2_3_4_4_2_3_3", "TRUE !PLAYER_PLAYING", Logger::Info);
			return true;
		}
		if (hbm.get()!=0 && hbm->ballfound()==0){
			Logger::Instance().WriteMsg("TrCond_ApproachBallTO0_3_2_3_4_4_2_3_3", "TRUE ballfound==0", Logger::Info);
			return true;
		}
		if ( obsm.get() != 0 ){

			int side;
			float posx=0.117, posy=0.03;
			
			side = (obsm->ball().bearing() > 0) ? 1 : -1;
			if ((fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.012 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <= 0.012) ){				
				Logger::Instance().WriteMsg("TrCond_ApproachBallTO0_3_2_3_4_4_2_3_3", "TRUE", Logger::Info);
				return true;
			}else{
				Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOApproachBall", "FALSE", Logger::Info);
				return false;
			}
		}
		return false;
    }
};
		
