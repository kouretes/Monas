
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// _open_group_play_or_NoPlay_close_group__TO_0.3.3
class TrCond_TesterGUI0_3_20_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage.player_state()==PLAYER_FINISHED */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_621149599->player_state()==PLAYER_FINISHED", _toString(var_621149599->player_state()==PLAYER_FINISHED),  Logger::Info);
		return ( var_621149599->player_state()==PLAYER_FINISHED ); 

		
    }
};
		