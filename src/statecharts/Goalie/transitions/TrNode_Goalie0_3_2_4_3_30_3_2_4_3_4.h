
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// GoToPosition_one_or_more_times_TO0.3.2.4.3.4
class TrCond_Goalie0_3_2_4_3_30_3_2_4_3_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage== NULL|| behavior.State.GameStateMessage.player_state()==PLAYER_READY || !robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 )
			 return true;
 		if ( var_1071592760.get() != 0  && var_1868674971.get() != 0 ){	
				return (  var_621149599->player_state()==PLAYER_READY || !robotInPosition(var_1071592760, var_1868674971) );
		}return false;

		
    }
};
		