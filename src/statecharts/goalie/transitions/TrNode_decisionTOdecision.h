
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
			
class TrCond_decisionTOdecision : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
	}

	bool Eval() {
		/* !PLAYER_FINISHED */
	//	Logger::Instance().WriteMsg("decision repeat", "!FINISHED ",Logger::Info);
	//	bool pen;
	//	pen = false;
		//boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		//if(gsm.get()!=0 && gsm->player_state()==PLAYER_PENALIZED)
	//		pen=true;
			
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		return true;
		//gsm = _blk->readState<GameStateMessage> ("behavior");
		//if(gsm.get()==0)
			//return true;
		////if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING){
			 ////BToHeadMessage* bmsg = new BToHeadMessage();
			////bmsg->set_headaction(CALIBRATE);
			
			////}
			
		//if(gsm.get()!=0 && gsm->player_state()!=PLAYER_FINISHED)
			//return true;
		//return false;
		//if()
    }
};
		
