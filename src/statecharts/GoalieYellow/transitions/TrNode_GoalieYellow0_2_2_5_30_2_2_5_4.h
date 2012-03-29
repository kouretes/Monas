
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// Localize_one_or_more_times__TO_GoToPosition_one_or_more_times_
class TrCond_GoalieYellow0_2_2_5_30_2_2_5_4 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.LocalizeDone.moveon()  */
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
	
				return ( (var_373152964.get()!=0 && var_373152964->moveon() ) );	
    }
};
		