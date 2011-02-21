
#include "architecture/statechartEngine/ICondition.h"
#include "architecture/archConfig.h"
#include "tools/toString.h"
#include "tools/logger.h"	
#include "messages/BehaviorMessages.pb.h"	
			
class TrCond_0_3_2_3_4_4_3_1TOSearchBall : public statechart_engine::ICondition {
			
public:

	
	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		bool cond;
	
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
	 
		if (hbm.get()==0 ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_4_3_1TOSearchBall", "TRUE NO HBM", Logger::Info);
			return true;
		}else{
			cond  = (hbm->ballfound()==0 ); 
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_4_3_1TOSearchBall", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
