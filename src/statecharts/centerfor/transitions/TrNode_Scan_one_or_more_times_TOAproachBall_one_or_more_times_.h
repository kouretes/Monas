
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"	
			
class TrCond_Scan_one_or_more_times_TOAproachBall_one_or_more_times_ : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		bool cond;
		
		boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
	
		if (gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			return true;
		}else if (hbm.get()==0 ){			
			//Logger::Instance().WriteMsg("TrCond_Scan_one_or_more_times_TOAproachBall_one_or_more_times_", "FALSE NO HBM", Logger::Info);
			return false;
		}else{
			cond  = (!(hbm->ballfound()==0 ) ); 
			//Logger::Instance().WriteMsg("TrCond_Scan_one_or_more_times_TOAproachBall_one_or_more_times_", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
