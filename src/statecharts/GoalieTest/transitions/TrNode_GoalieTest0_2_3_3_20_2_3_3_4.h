
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.3.3.2TOGoToPosition_one_or_more_times_
class TrCond_GoalieTest0_2_3_3_20_2_3_3_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.Signal.ReturnToPositionMessage.goalietopos() */
		
		boost::shared_ptr<const ReturnToPositionMessage> var_2127569193 = _blk->readSignal<ReturnToPositionMessage> ("behavior" );

		
		if ( var_2127569193.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_2127569193->goalietopos()", _toString(var_2127569193->goalietopos()),  Logger::Info);
		return ( var_2127569193->goalietopos() ); 

		
    }
};
		