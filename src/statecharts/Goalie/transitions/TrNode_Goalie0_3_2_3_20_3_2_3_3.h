
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.3.2.3.2TOScan_one_or_more_times_
class TrCond_Goalie0_3_2_3_20_3_2_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("_behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.HeadToBMessage.ballfound()==0 */
		
		boost::shared_ptr<const HeadToBMessage> var_1318191046 = _blk->readState<HeadToBMessage> ("_behavior" );

		
		if ( var_1318191046.get() != 0  ){	
				return ( var_1318191046->ballfound()==0 );
		}return false;

		
    }
};
		