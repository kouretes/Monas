
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// LocalizeTO0.2.4.2.3.3
class TrCond_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worlstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worlstate.State.GameStateMessage==NULL || worlstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING || behavior.State.LocalizeDone.moveon() */
		boost::shared_ptr<const GameStateMessage> var_492908884 = _blk->readState<GameStateMessage> ("worlstate" );
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
	
				return ( var_492908884.get()==0 ||  (var_492908884.get()!=0 && var_492908884->player_state()!=PLAYER_PLAYING) || (var_373152964.get()!=0 && var_373152964->moveon()) );	
    }
};
		