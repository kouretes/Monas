
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.3.6.3.2TOSpecialAction
class TrCond_Goalie0_3_2_3_6_3_20_3_2_3_6_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING &&
 behavior.State.FallMessage.fall()!=0 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const FallMessage> var_1561961811 = _blk->readState<FallMessage> ("behavior" );
	
				return ( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) &&  (var_1561961811.get()!=0 && var_1561961811->fall()!=0) );	
    }
};
		