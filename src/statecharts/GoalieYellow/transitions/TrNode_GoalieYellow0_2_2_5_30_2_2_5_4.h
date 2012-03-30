
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// Localize_one_or_more_times__TO_GoToPosition_one_or_more_times_
class TrCond_GoalieYellow0_2_2_5_30_2_2_5_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.State.LocalizeDone.moveon()  */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_373152964.get()!=0 && var_373152964->moveon() ) );	
    }
};
		