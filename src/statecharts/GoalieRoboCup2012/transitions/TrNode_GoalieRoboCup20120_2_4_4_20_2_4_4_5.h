
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.4.4.2TOKick_one_or_more_times_
class TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_5 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.HeadToBMessage.ballfound()>0 &&
 readyToKick(worldstate.Data.WorldInfo) */
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
	
				return ( (var_1901744185.get()!=0 && var_1901744185->ballfound()>0) &&  readyToKick(var_1111221333) );	
    }
};
		