
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "tools/BehaviorConst.h"

class TrCond_GoToPositionTOGoToPosition : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		//Logger::Instance().WriteMsg("TrCond_GoToPositionTOGoToPosition", " ", Logger::Info);
		/* (PLAYER_READY and !inPosition  ) ||(PLAYER_PLAYING and !inPosition)*/
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("worldstate");
		boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("worldstate");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");


		
		if(gsm.get()!=0 && (gsm->player_state()==PLAYER_READY || gsm->player_state() == PLAYER_PLAYING)){
			if(pm.get()!=0 && wimsg.get()!=0){
				return (! robotInPosition( wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(),pm->posy(), wimsg->myposition().phi(), pm->theta()));
			}
		}
		//if(gsm.get()!=0 &&  gsm->player_state()==PLAYER_PLAYING){
			//if(pm.get()!=0 && wimsg.get()!=0){
				//int side =1;
				//if(pm->posx()<0)
					//side = -1;
				//if(fabs(wimsg->myposition().y()) <0.3 && side*wimsg->myposition().x() > 2.4){  //polu makria apo thn perioxh tou termatofulaka
					//return false;
				//}else
					//return true;
			//}
		//}
		return false;
    }
};

