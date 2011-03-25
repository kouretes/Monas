
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"		
#include "tools/toString.h"
#include "tools/logger.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
#include "messages/BehaviorMessages.pb.h"		
			
class TrCond_findBall_one_or_more_times_TOkick : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("vision", 0);
		_blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("behavior");		
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			Logger::Instance().WriteMsg("TrCond_findBall_one_or_more_times_TOkick", "FALSE !PLAYER_PLAYING", Logger::Info);
			return true;	
		}
		if(hbm.get()==0 ||(hbm.get()!=0 && hbm->ballfound()==0)){
			Logger::Instance().WriteMsg("TrCond_findBall_one_or_more_times_TOkick", "FALSE NO OBSM, HBM", Logger::Info);
			return false;
		}
		else{
			Logger::Instance().WriteMsg("TrCond_findBall_one_or_more_times_TOkick", "TRUE", Logger::Info);
			return true;
		}
		//if ( obsm.get() != 0 ){
			//int side;
			//float posx=0.117, posy=0.03;
			
			//side = (obsm->ball().bearing() > 0) ? 1 : -1;
			//Logger::Instance().WriteMsg("Aproach ", "X: "+ _toString(obsm->ball().dist() * cos(obsm->ball().bearing()) - posx)+" Y: "+ _toString( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy)),Logger::Info);
			//if ( (fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.01 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <= 0.01) ){
				//Logger::Instance().WriteMsg("TrCond_AproachBall_one_or_more_times_TOMyLeftSideKick", "TRUE", Logger::Info);
				//return true;
			//}else{
				//Logger::Instance().WriteMsg("TrCond_AproachBall_one_or_more_times_TOMyLeftSideKick", "FALSE", Logger::Info);
				//return false;
			//}
		////	return true;
		//}
		//return false;
		
    }
};
		

