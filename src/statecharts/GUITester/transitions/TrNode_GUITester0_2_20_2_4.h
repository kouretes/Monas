
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.2TONoPlay
class TrCond_GUITester0_2_20_2_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );

		Logger::Instance().WriteMsg("0.2.2TONoPlay, worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING" ,_toString(var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING)),  Logger::Info);
		return ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING) );
    }
};
		