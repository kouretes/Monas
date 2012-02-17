
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.2TOnonPlaying
class TrCond_Goalie0_3_2_20_3_2_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("_behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("_behavior" );

		
		if ( var_1692513826.get() != 0  ){	
				return ( var_1692513826->player_state()!=PLAYER_PLAYING );
		}return false;

		
    }
};
		