
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
// 0.2.2TOPlaying
class TrCond_GoalieRoboCup20120_2_20_2_4 : public statechart_engine::ICondition
{
public:
	void UserInit ()
	{
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
	}
	bool Eval()
	{
		/* worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		return ( (var_1263977940.get() != 0 && var_1263977940->player_state() == PLAYER_PLAYING) );
	}
};
