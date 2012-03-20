
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// _open_group_playing_or_nonPlaying_close_group__one_or_more_times__TO_0.3
class TrCond_GoaliePenalty0_20_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/*  worldstate.State.GameStateMessage.player_state()==PLAYER_FINISHED */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );

		Logger::Instance().WriteMsg("_open_group_playing_or_nonPlaying_close_group__one_or_more_times__TO_0.3,  worldstate.State.GameStateMessage.player_state()==PLAYER_FINISHED" ,_toString( (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_FINISHED)),  Logger::Info);
		return (  (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_FINISHED) );
    }
};
		