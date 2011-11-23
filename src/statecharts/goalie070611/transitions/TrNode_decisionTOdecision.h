
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
class TrCond_decisionTOdecision : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* !PLAYER_FINISHED */
		//Logger::Instance().WriteMsg("decision forever", "!FINISHED ",Logger::Info);
		bool pen;
		pen = false;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("behavior");
		if(gsm.get()!=0 && (gsm->player_state()==PLAYER_PENALISED || gsm->player_state()==PLAYER_INITIAL))
			pen=true;
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();

		gsm = _blk->readState<GameStateMessage> ("behavior");
		if(gsm.get()==0)
			return true;
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING && pen){
			BToHeadMessage* bmsg = new BToHeadMessage();
			bmsg->set_headaction(CALIBRATE);
			_blk->publishSignal(*bmsg, "behavior");
			//////////////////////////resetttttttttttttttttttttt
			LocalizationResetMessage* r = new LocalizationResetMessage();
			_blk->publishSignal(*r, "behavior");
			_blk->publish_all();
			SysCall::_usleep(5000000);
			_blk->process_messages();
			pen = false;
			}
		if(gsm->player_state()!=PLAYER_FINISHED)
			return true;
		return false;
    }
};

