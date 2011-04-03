
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
			
class TrCond_0_3_2_2TONoPlay : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* !PLAYER_PLAYING */
		//Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()==0){
		//	RestartTurnMessage rtm;
		//	rtm.set_restartnow(true);
		//	_blk->publishSignal(rtm,"behavior");
			Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay", "RESTARTTTTTTTTTTTTTTT", Logger::Info);
			return true;
		}
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//	RestartTurnMessage rtm;
		//	rtm.set_restartnow(true);
		//	_blk->publishSignal(rtm,"behavior");
			Logger::Instance().WriteMsg("TrCond_0_3_2_2TONoPlay", "RESTARTTTTTTTTTTTTTTT", Logger::Info);
			return true;
		}
		return false;
	}
};
		
