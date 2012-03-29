
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.6.2_TO_Localize_one_or_more_times_
class TrCond_GoalieYellow0_2_2_5_20_2_2_5_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && worldstate.State.GameStateMessage.previous_player_state()==PLAYER_PENALISED */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_1263977940.get()!=0 && var_1263977940->previous_player_state()==PLAYER_PENALISED) );	
    }
};
		