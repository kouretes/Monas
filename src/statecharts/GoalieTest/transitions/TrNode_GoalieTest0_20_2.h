
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// decisionTOdecision
class TrCond_GoalieTest0_20_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.GameStateMessage.player_state()!=PLAYER_FINISHED */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );

		
		if ( var_1692513826.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("var_1692513826->player_state()!=PLAYER_FINISHED", _toString(var_1692513826->player_state()!=PLAYER_FINISHED),  Logger::Info);
		return ( var_1692513826->player_state()!=PLAYER_FINISHED ); 

		
    }
};
		