
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// GoToPositionTO0.2.3.3.3
class TrCond_GUITester0_2_3_3_20_2_3_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage==NULL || behavior.State.GameStateMessage.player_state()!=PLAYER_PLAYING || robotInPosition( behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );

		
		if ( var_621149599.get() == 0 )
			 return true;
 		if ( var_1071592760.get() != 0  && var_1868674971.get() != 0 ){	
				return (  var_621149599->player_state()!=PLAYER_PLAYING || robotInPosition( var_1071592760, var_1868674971) );
		}return false;

		
    }
};
		