
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"			
class TrCond_0_3_2_3_2TOsearchBallOrReturnToPosition : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* ballfound==0 */
		Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOsearchBallOrReturnToPosition", "  ", Logger::Info);
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");

		if(hbm.get()==0)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()==0)
			return true;
		return false;
    }
};
		
