
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.2TO_open_group_NoPlay_sequence__open_option_GoToPosition_one_or_more_times__close_option__close_group_
class TrCond_GoalieTest0_2_20_2_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );

		
		if ( var_1692513826.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("var_1692513826->player_state()!=PLAYER_PLAYING", _toString(var_1692513826->player_state()!=PLAYER_PLAYING),  Logger::Info);
		return ( var_1692513826->player_state()!=PLAYER_PLAYING ); 

		
    }
};
		