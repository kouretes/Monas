
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"		
#include "architecture/archConfig.h"		
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_4_5_2TOMyRightKick : public statechart_engine::ICondition {
			
public:
	void UserInit () { 
		_blk->subscribeTo("vision", 0);
      }

	bool Eval() {
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		 if (obsm.get()!=0 && (obsm->ball().dist() * sin(obsm->ball().bearing()) >0.0) ) {
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_4_2TOMyRightKick", "FALSE", Logger::Info);
			return false;
		}else{
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_4_2TOMyRightKick", "TRUE", Logger::Info);
			return true;
		}
    } 
};
