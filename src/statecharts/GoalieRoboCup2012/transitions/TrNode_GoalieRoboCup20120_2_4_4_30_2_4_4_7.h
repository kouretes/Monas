
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// Scan_one_or_more_times_TO0.2.4.4.7
class TrCond_GoalieRoboCup20120_2_4_4_30_2_4_4_7 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.HeadToBMessage.ballfound()>0 ||
 worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING || worldstate.State.GameStateMessage==NULL */
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
	
				return ( (var_1901744185.get()!=0 && var_1901744185->ballfound()>0) ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_PLAYING) || var_1263977940.get()==0  );	
    }
};
		