
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.4.2.2TOLocalize_one_or_more_times_
class TrCond_GoalieRoboCup20120_2_4_2_20_2_4_2_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.previous_player_state()==PLAYER_PENALISED && !behavior.State.LocalizeDone.moveon() */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->previous_player_state()==PLAYER_PENALISED) && (var_373152964.get()!=0 && !var_373152964->moveon()) );	
    }
};
		