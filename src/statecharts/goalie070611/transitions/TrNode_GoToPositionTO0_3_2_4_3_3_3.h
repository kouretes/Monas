
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/GoToPosition/GoToPosition.h"
#include "activities/BehaviorConst.h"

class TrCond_GoToPositionTO0_3_2_4_3_3_3 : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* !PLAYER_READY || inPosition */
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");

		GoToPosition g;
		bool ret = false;
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_READY)
			ret= true;

		if(pm.get()!=0 && wimsg.get()!=0){
			//if(wimsg->myposition().confidence()<badConfidence){
				//Logger::Instance().WriteMsg("TrCond_GoToPositionTO0_3_2_4_3_3_3", "  confidence " + _toString( wimsg->myposition().confidence()), Logger::Info);
				//return true;
			//}
			ret =  g.robotInPosition(pm->posx(), wimsg->myposition().x(), pm->posy(), wimsg->myposition().y(), pm->theta(), wimsg->myposition().phi());
		}
		if(ret){
		//	Logger::Instance().WriteMsg("robotIposition",  " stop walking", Logger::Info);
			MotionActionMessage* amot = new MotionActionMessage();
			amot->set_command("Init.xar");
			_blk->publishSignal(*amot, "behavior");
			return true;
		}
		return false;

    }
};

