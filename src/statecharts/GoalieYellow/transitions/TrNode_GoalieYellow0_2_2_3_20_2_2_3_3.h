
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.3.2_TO_Scan_one_or_more_times_
class TrCond_GoalieYellow0_2_2_3_20_2_2_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* worldstate.State.GameStateMessage==NULL || 
behavior.State.HeadToBMessage.ballfound()==0 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
	
				return ( var_1263977940.get()==0 ||   (var_1901744185.get()!=0 && var_1901744185->ballfound()==0) );	
    }
};
		