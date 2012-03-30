
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// nonPlaying_TO_0.2.2.6
class TrCond_GoalieYellow0_2_2_40_2_2_6 : public statechart_engine::ICondition {		
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

		Logger::Instance().WriteMsg("nonPlaying_TO_0.2.2.6, worldstate.State.GameStateMessage== NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_READY  || robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage)" ,_toString(var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_READY)  || robotInPosition(var_1071592760, var_1868674971)),  Logger::Info);
		return ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_READY)  || robotInPosition(var_1071592760, var_1868674971) );
    }
};
		