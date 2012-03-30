
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.3.2.4.3.2_TO_GoToPosition_one_or_more_times_
class TrCond_GoalieYellow0_2_2_4_3_20_2_2_4_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_READY */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );

		Logger::Instance().WriteMsg("0.3.2.4.3.2_TO_GoToPosition_one_or_more_times_, worldstate.State.GameStateMessage.player_state()==PLAYER_READY" ,_toString((var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_READY)),  Logger::Info);
		return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_READY) );
    }
};
		