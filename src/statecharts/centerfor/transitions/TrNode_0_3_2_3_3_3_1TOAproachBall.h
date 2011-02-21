
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"		
#include "tools/toString.h"
#include "tools/logger.h"	
		
class TrCond_0_3_2_3_3_3_1TOAproachBall : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
		if ( hbm.get() == 0 ){
			
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_3_3_1TOAproachBall", "FALSE NO HBM", Logger::Info);
			return false;
		}else{
			bool cond = (hbm->ballfound()>0);
			return cond;
		}
		//return false;
    }
		
};
		
