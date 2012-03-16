
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.2TONoPlay
class TrCond_TestBehavior0_2_20_2_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.GameStateMessage==NULL||behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
	
				return ( var_621149599.get()==0 || (var_621149599.get()!=0 && var_621149599->player_state()!=PLAYER_PLAYING) );	
    }
};
		