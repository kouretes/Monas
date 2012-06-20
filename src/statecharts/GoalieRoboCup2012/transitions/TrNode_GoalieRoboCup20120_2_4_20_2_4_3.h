
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_option_Localize_one_or_more_times__close_option_TO_open_option_GoToPosition_one_or_more_times__close_option_
class TrCond_GoalieRoboCup20120_2_4_20_2_4_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.LocalizeDone.moveon()||worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.previous_player_state()!=PLAYER_PENALISED */
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
	
				return ( (var_373152964.get()!=0 && var_373152964->moveon())|| var_1263977940.get()==0  || (var_1263977940.get()!=0 && var_1263977940->previous_player_state()!=PLAYER_PENALISED) );	
    }
};
		