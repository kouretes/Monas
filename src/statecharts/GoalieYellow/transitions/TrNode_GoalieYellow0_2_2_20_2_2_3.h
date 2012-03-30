
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.2_TO_playing
class TrCond_GoalieYellow0_2_2_20_2_2_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING 
&& (behavior.State.ReturnToPositionMessage.goalietopos()
|| worldstate.State.GameStateMessage.previous_player_state()!=PLAYER_PENALISED) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const ReturnToPositionMessage> var_969154074 = _blk->readState<ReturnToPositionMessage> ("behavior" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING)  && ((var_969154074.get()!=0 && var_969154074->goalietopos() )|| (var_1263977940.get()!=0 && var_1263977940->previous_player_state()!=PLAYER_PENALISED)) );	
    }
};
		