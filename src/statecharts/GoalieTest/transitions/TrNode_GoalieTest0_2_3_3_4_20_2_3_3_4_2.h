
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// GoToPositionTOGoToPosition
class TrCond_GoalieTest0_2_3_3_4_20_2_3_3_4_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("localization",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(localization) && behavior.State.GameStateMessage.player_state()==PLAYER_PLAYING && !robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("localization");

		
		_blk->process_messages();
		if( msg.get() == 0){
			Logger::Instance().WriteMsg("TIMEOUT", "true",  Logger::Info);
		return true;
		}
 		if(msg->wakeup()=="")
			return true;
		if ( var_621149599.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_1071592760.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_1868674971.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}

		
		Logger::Instance().WriteMsg("boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && !robotInPosition(var_1071592760, var_1868674971)", _toString(boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && !robotInPosition(var_1071592760, var_1868674971)),  Logger::Info);
		return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && !robotInPosition(var_1071592760, var_1868674971) ); 

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// GoToPositionTOGoToPosition
class TrAction_GoalieTest0_2_3_3_4_20_2_3_3_4_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.localization.250 */
	public:	TrAction_GoalieTest0_2_3_3_4_20_2_3_3_4_2() : statechart_engine::TimeoutAction( "localization", 250 ) { 
		;
	 }
};

		