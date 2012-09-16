
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
// Stare_TO_0.2.2.3.4.2.3
class TrCond_GoaliePenalty0_2_2_3_4_2_20_2_2_3_4_2_3 : public statechart_engine::ICondition
{
public:
	void UserInit ()
	{
		_blk->updateSubscription("worldstate", msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}
	bool Eval()
	{
		/* worldstate.State.GameStateMessage==NULL||
		worldstate.State.GameStateMessage.player_state()!=PLAYER_PLAYING
		|| worldstate.State.GameStateMessage.kickoff()
		|| behavior.State.HeadToBMessage.ballfound()==0
		||  behavior.State.FallMessage.fall()!=0 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const FallMessage> var_1561961811 = _blk->readState<FallMessage> ("behavior" );
		return ( var_1263977940.get() == 0 ||  (var_1263977940.get() != 0 && var_1263977940->player_state() != PLAYER_PLAYING) || (var_1263977940.get() != 0 && var_1263977940->kickoff()  ) || (var_1901744185.get() != 0 && var_1901744185->ballfound() == 0)  ||  (var_1561961811.get() != 0 && var_1561961811->fall() != 0) );
	}
};
