
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"					
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_AproachBallTO0_3_2_3_3_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {

		boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			return true;
		}else if ( obsm.get() == 0 ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "FALSE NO OBSM, HBM", Logger::Info);
			return true;
		}else{
			int side;
			float posx=0.117, posy=0.03;
			
			side = (obsm->ball().bearing() > 0) ? 1 : -1;
			if ((fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.01 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <= 0.01) ){				
				//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "TRUE", Logger::Info);
				return true;
			}else{
			//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "FALSE", Logger::Info);
				return false;
			}
		}
    }
};
		
