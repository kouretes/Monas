
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.4.4.2TOScan_one_or_more_times_
class TrCond_GoalieRoboCup20120_2_4_4_20_2_4_4_3 : public statechart_engine::ICondition {		
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
		