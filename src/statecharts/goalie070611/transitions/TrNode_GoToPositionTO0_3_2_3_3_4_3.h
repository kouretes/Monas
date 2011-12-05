
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/GoToPosition/GoToPosition.h"
#include "activities/BehaviorConst.h"

class TrCond_GoToPositionTO0_3_2_3_3_4_3 : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* inPosition */
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		GoToPosition g;
		bool ret = false;
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return true;
		//if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING && hbm.get()!=0 && hbm->ballfound()!=0)
			//return true;
		//boost::shared_ptr<const ReturnToPositionMessage> rpm = _blk->readSignal<ReturnToPositionMessage>("behavior");
		//if(rpm!=0 && !rpm->goalietopos())
			//return true;
		if(pm.get()!=0 && wimsg.get()!=0){
			//if(wimsg->myposition().confidence()<badConfidence){
				//Logger::Instance().WriteMsg("TrCond_GoToPositionTO0_3_2_3_3_4_3", "  confidence " + _toString( wimsg->myposition().confidence()), Logger::Info);
				//return true;
			////}
			int side =1;
			if(pm->posx()<0)
				side = -1;
			if(fabs(wimsg->myposition().y()) <0.3 && side*wimsg->myposition().x() > 2.4){  //polu makria apo thn perioxh tou termatofulaka
				//Logger::Instance().WriteMsg("TrCond_GoToPositionTO0_3_2_3_3_4_3", "Not near the goal", Logger::Info);
				//return true;
				ret = true;
			}
		//	ret = g.robotInPosition( wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(),pm->posy(), wimsg->myposition().phi(), pm->theta());
			if(ret){
			//	Logger::Instance().WriteMsg("robotIposition",  " stop walking", Logger::Info);
				MotionActionMessage* amot = new MotionActionMessage();
				amot->set_command("Init.xar");
				_blk->publishSignal(*amot, "behavior");
				return true;
			}
		}
		return false;
    }
};

