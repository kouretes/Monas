
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/BehaviorConst.h"
// Localize_one_or_more_times_TO0.2.4.2.4
class TrCond_GoalieRoboCup20120_2_4_2_30_2_4_2_4 : public statechart_engine::ICondition
{
public:
	void UserInit ()
	{
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}
	bool Eval()
	{
		/* behavior.State.LocalizeDone.moveon() */
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
		return ( (var_373152964.get() != 0 && var_373152964->moveon()) );
	}
};
