
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_group_play_or_NoPlay_close_group_TO0.3
class TrCond_TestBehavior0_20_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.GameStateMessage.player_state()==PLAYER_FINISHED */
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
	
				return ( (var_621149599.get()!=0 && var_621149599->player_state()==PLAYER_FINISHED) );	
    }
};
		