
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.3.3.2TOScan_one_or_more_times_
class TrCond_GoalieTest0_2_3_3_20_2_3_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* !_behavior.Signal.ReturnToPositionMessage.goalietopos() */
		
		boost::shared_ptr<const ReturnToPositionMessage> var_756251670 = _blk->readSignal<ReturnToPositionMessage> ("behavior" );

		
		if ( var_756251670.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("!var_756251670->goalietopos()", _toString(!var_756251670->goalietopos()),  Logger::Info);
		return ( !var_756251670->goalietopos() ); 

		
    }
};
		