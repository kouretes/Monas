
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
// GoToPositionTO0.2.4.3.3.3
class TrCond_GoalieRoboCup20120_2_4_3_3_20_2_4_3_3_3 : public statechart_engine::ICondition
{
public:
	void UserInit ()
	{
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}
	bool Eval()
	{
		/* worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING
		|| robotInPosition(worldstate.Data.WorldInfo, behavior.State.PositionMessage)
		||behavior.State.ReturnToPositionMessage.goalietopos() */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		boost::shared_ptr<const ReturnToPositionMessage> var_969154074 = _blk->readState<ReturnToPositionMessage> ("behavior" );
		return ( var_1263977940.get() == 0 ||  (var_1263977940.get() != 0 && var_1263977940->player_state() != PLAYER_PLAYING) || robotInPosition(var_1111221333, var_1868674971) || (var_969154074.get() != 0 && var_969154074->goalietopos()) );
	}
};
