
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.2TOplay
class TrCond_GoalieTest0_2_20_2_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage.player_state()==PLAYER_PLAYING */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_621149599->player_state()==PLAYER_PLAYING", _toString(var_621149599->player_state()==PLAYER_PLAYING),  Logger::Info);
		return ( var_621149599->player_state()==PLAYER_PLAYING ); 

		
    }
};
		