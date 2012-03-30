
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// Localize_one_or_more_times__TO_0.2.2.5.5
class TrCond_GoalieYellow0_2_2_5_30_2_2_5_5 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL 
|| worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );

		Logger::Instance().WriteMsg("Localize_one_or_more_times__TO_0.2.2.5.5, worldstate.State.GameStateMessage==NULL  || worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING" ,_toString(var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING)),  Logger::Info);
		return ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING) );
    }
};
		