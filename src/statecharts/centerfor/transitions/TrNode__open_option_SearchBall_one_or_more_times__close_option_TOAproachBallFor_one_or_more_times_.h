
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_ : public statechart_engine::ICondition {
			
public:

	
	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		bool cond;
		
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
		
		if (hbm.get()==0 ){			
			//Logger::Instance().WriteMsg("TrCond__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_", "FALSE NO HBM", Logger::Info);
			return false;
		}else{
			cond  = (!(hbm->ballfound()==0 ) ); 
			//Logger::Instance().WriteMsg("TrCond__open_option_SearchBall_one_or_more_times__close_option_TOAproachBallFor_one_or_more_times_", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
