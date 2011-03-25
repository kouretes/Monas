
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"		
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_ApproachBallTOApproachBall : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(100000);
		_blk->process_messages();
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("behavior");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			//Logger::Instance().WriteMsg("TrCond_ApproachBallTOApproachBall", "FALSE !PLAYING", Logger::Info);
			return false;
		}
		if (hbm.get()!=0 && hbm->ballfound()==0)
			return false;
		if ( obsm.get() != 0 ){
			int side;
			float posx=0.117, posy=0.03;
			
			side = (obsm->ball().bearing() > 0) ? 1 : -1;
			Logger::Instance().WriteMsg("Approach ", "X: "+ _toString(obsm->ball().dist() * cos(obsm->ball().bearing()) - posx)+" Y: "+ _toString( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy)),Logger::Info);
			 if ( (fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) > 0.012 || fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) > 0.012) ){
				Logger::Instance().WriteMsg("TrCond_ApproachBallTOApproachBall", "TRUE", Logger::Info);
				return true;
			}else{
				Logger::Instance().WriteMsg("TrCond_ApproachBallTOApproachBall", "FALSE", Logger::Info);
				return false;
			}
		}
		return true;
    }
};
		
