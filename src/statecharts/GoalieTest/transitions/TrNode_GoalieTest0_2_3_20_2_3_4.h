
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// 0.2.3.2TOApproachBall_one_or_more_times_
class TrCond_GoalieTest0_2_3_20_2_3_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.HeadToBMessage.ballfound()!=0 && !ballAway(behavior.Data.WorldInfo) && !readyToKick(behavior.Data.WorldInfo) */
		
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );

		
		if ( var_1901744185.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_1071592760.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("var_1901744185->ballfound()!=0 && !ballAway(var_1071592760) && !readyToKick(var_1071592760)", _toString(var_1901744185->ballfound()!=0 && !ballAway(var_1071592760) && !readyToKick(var_1071592760)),  Logger::Info);
		return ( var_1901744185->ballfound()!=0 && !ballAway(var_1071592760) && !readyToKick(var_1071592760) ); 

		
    }
};
		