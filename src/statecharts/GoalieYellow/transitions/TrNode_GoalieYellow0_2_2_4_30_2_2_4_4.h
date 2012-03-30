
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_option_GoToPosition_one_or_more_times__close_option__TO_0.2.2.4.4
class TrCond_GoalieYellow0_2_2_4_30_2_2_4_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage== NULL
|| worldstate.State.GameStateMessage.player_state()!=PLAYER_READY 
|| robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
	
				return ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_READY)  || robotInPosition(var_1071592760, var_1868674971) );	
    }
};
		