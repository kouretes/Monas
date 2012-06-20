
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.4.3.2TOGoToPosition_one_or_more_times_
class TrCond_GoalieRoboCup20120_2_4_3_20_2_4_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.previous_player_state()==PLAYER_PENALISED
&& !behavior.State.ReturnToPositionMessage.goalietopos() */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const ReturnToPositionMessage> var_969154074 = _blk->readState<ReturnToPositionMessage> ("behavior" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->previous_player_state()==PLAYER_PENALISED) && (var_969154074.get()!=0 && !var_969154074->goalietopos()) );	
    }
};
		