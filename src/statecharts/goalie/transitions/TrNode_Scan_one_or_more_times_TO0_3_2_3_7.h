
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h""
			
class TrCond_Scan_one_or_more_times_TO0_3_2_3_7 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* ballfound!=0 */
		Logger::Instance().WriteMsg("TrCond_Scan_one_or_more_times_TO0_3_2_3_7", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()!=0)
			return true;
		return false;
    }
};		
