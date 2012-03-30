
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// Localize_TO_0.2.2.5.3.3
class TrCond_GoalieYellow0_2_2_5_3_20_2_2_5_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.LocalizeDone.moveon() || worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
	
				return ( (var_373152964.get()!=0 && var_373152964->moveon() )|| var_1263977940.get()==0  || (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING) );	
    }
};
		