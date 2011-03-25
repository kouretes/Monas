
#include "messages/BehaviorMessages.pb.h"
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"		
#include "activities/ApproachBall/ApproachBall.h"			
			
class TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4 : public statechart_engine::ICondition {
			
public:
void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {

		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("behavior");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("vision");
		ApproachBall ab;
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING || ( hbm.get()!=0 && hbm->ballfound()==0  && obsm.get()!=0 && ab.readyToKick(obsm)==true)){
			Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4", "TRUE NO GSM", Logger::Info);
			return true;
		}
		
		return false;
    }
};
		

