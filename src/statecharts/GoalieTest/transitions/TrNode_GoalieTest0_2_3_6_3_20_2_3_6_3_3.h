
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.3.6.3.2TOSpecialAction
class TrCond_GoalieTest0_2_3_6_3_20_2_3_6_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.Signal.FallMessage.fall()!=0 */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const FallMessage> var_734099742 = _blk->readSignal<FallMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_734099742.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_621149599->player_state()==PLAYER_PLAYING && var_734099742->fall()!=0", _toString(var_621149599->player_state()==PLAYER_PLAYING && var_734099742->fall()!=0),  Logger::Info);
		return ( var_621149599->player_state()==PLAYER_PLAYING && var_734099742->fall()!=0 ); 

		
    }
};
		