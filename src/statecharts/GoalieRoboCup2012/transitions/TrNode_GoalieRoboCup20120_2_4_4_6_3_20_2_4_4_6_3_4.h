
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
// 0.2.4.4.6.3.2TO0.2.4.4.6.3.4
class TrCond_GoalieRoboCup20120_2_4_4_6_3_20_2_4_4_6_3_4 : public statechart_engine::ICondition
{
public:
	void UserInit ()
	{
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}
	bool Eval()
	{
		/* worldstate.State.GameStateMessage==NULL ||
		worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING ||behavior.State.FallMessage==NULL || behavior.State.FallMessage.fall()==0 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const FallMessage> var_1561961811 = _blk->readState<FallMessage> ("behavior" );
		return ( var_1263977940.get() == 0 ||   (var_1263977940.get() != 0 && var_1263977940->player_state() != PLAYER_PLAYING) || var_1561961811.get() == 0  || (var_1561961811.get() != 0 && var_1561961811->fall() == 0) );
	}
};
