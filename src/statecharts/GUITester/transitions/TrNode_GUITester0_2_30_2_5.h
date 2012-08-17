
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
#include "tools/logger.h"
#include "tools/toString.h"
// playTO0.2.5
class TrCond_GUITester0_2_30_2_5 : public statechart_engine::ICondition
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
		worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING ||
		robotInPosition( worldstate.Data.WorldInfo, behavior.State.PositionMessage) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		Logger::Instance().WriteMsg("playTO0.2.5, worldstate.State.GameStateMessage==NULL ||  worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING ||  robotInPosition( worldstate.Data.WorldInfo, behavior.State.PositionMessage)" , _toString(var_1263977940.get() == 0 ||   (var_1263977940.get() != 0 && var_1263977940->player_state() != PLAYER_PLAYING) ||  robotInPosition( var_1111221333, var_1868674971)),  Logger::Info);
		return ( var_1263977940.get() == 0 ||   (var_1263977940.get() != 0 && var_1263977940->player_state() != PLAYER_PLAYING) ||  robotInPosition( var_1111221333, var_1868674971) );
	}
};
