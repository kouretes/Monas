
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_ScanTOScan : public statechart_engine::ICondition {
			
public:
	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* ballfound==0 */
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		//Logger::Instance().WriteMsg("TrCond_ScanTOScan", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()==0)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()==0)
			return true;
		return false;
    }
};
		
