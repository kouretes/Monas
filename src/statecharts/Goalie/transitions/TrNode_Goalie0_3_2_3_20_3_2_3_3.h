
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.3.2TOScan_one_or_more_times_
class TrCond_Goalie0_3_2_3_20_3_2_3_3 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* behavior.State.GameStateMessage==NULL || 
behavior.State.HeadToBMessage.ballfound()==0 */
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
	
				return ( var_621149599.get()==0 ||   (var_1901744185.get()!=0 && var_1901744185->ballfound()==0) );	
    }
};
		