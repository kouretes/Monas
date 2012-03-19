
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.2TOnonPlaying
class TrCond_Goalie0_3_2_20_3_2_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL ||worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
	
				return ( var_1263977940.get()==0 || (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING) );	
    }
};
		