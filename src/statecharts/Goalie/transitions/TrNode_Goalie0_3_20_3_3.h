
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_group_playing_or_nonPlaying_close_group_TO0.3.3
class TrCond_Goalie0_3_20_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/*  behavior.State.GameStateMessage.player_state()==PLAYER_FINISHED */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );

		
		if ( var_621149599.get() != 0  ){	
				return (  var_621149599->player_state()==PLAYER_FINISHED );
		}return false;

		
    }
};
		