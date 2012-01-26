
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.3.2TOsearchBallOrGoToPosition
class TrCond_GoalieTest0_2_3_20_2_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* _behavior.State.HeadToBMessage.ballfound()==0 */
		
		boost::shared_ptr<const HeadToBMessage> var_1318191046 = _blk->readState<HeadToBMessage> ("behavior" );

		
		if ( var_1318191046.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("var_1318191046->ballfound()==0", _toString(var_1318191046->ballfound()==0),  Logger::Info);
		return ( var_1318191046->ballfound()==0 ); 

		
    }
};
		