
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// KickTO0.2.3.7
class TrCond_GoalieTest0_2_3_50_2_3_7 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) */
		
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");

		
		_blk->process_messages();
		if( msg.get() == 0){
			Logger::Instance().WriteMsg("TIMEOUT", "true",  Logger::Info);
		return true;
		}
 		if(msg->wakeup()=="")
			return true;

		
		Logger::Instance().WriteMsg("boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()", _toString(boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()),  Logger::Info);
		return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() ); 

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// KickTO0.2.3.7
class TrAction_GoalieTest0_2_3_50_2_3_7 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieTest0_2_3_50_2_3_7() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		