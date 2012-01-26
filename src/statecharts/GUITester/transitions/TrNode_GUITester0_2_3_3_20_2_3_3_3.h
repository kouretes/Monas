
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// GoToPositionTO0.2.3.3.3
class TrCond_GUITester0_2_3_3_20_2_3_3_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* robotInPosition( _behavior.Data.WorldInfo, _behavior.State.PositionMessage) */
		
		boost::shared_ptr<const WorldInfo> var_999081479 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1392547972 = _blk->readState<PositionMessage> ("behavior" );

		
		if ( var_999081479.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}
		if ( var_1392547972.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("robotInPosition( var_999081479, var_1392547972)", _toString(robotInPosition( var_999081479, var_1392547972)),  Logger::Info);
		return ( robotInPosition( var_999081479, var_1392547972) ); 

		
    }
};
		