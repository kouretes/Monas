
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"	

			
class TrCond_ScanForBall_one_or_more_times_TOWalktoPost : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		bool cond;		
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
	
		if (hbm.get()==0 ){			
			//Logger::Instance().WriteMsg("TrCond_ScanForBall_one_or_more_times_TOWalktoPost", "FLSE NO HBM", Logger::Info);
			return false;
		}else{
			cond  = (!(hbm->ballfound()==0 )); 
			//Logger::Instance().WriteMsg("TrCond_ScanForBall_one_or_more_times_TOWalktoPost", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
