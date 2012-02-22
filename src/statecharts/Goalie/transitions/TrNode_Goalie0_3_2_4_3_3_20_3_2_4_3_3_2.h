
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// GoToPositionTOGoToPosition
class TrCond_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) && behavior.State.GameStateMessage.player_state()==PLAYER_READY && robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");

		
		_blk->process_messages();
		if( msg.get() == 0){
			return true;
		}
 		if(msg->wakeup()=="")
			return true;

		if ( var_621149599.get() != 0  && var_1071592760.get() != 0 && var_1868674971.get() != 0 ){	
				return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_READY && robotInPosition(var_1071592760, var_1868674971) );
		}return false;

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// GoToPositionTOGoToPosition
class TrAction_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_Goalie0_3_2_4_3_3_20_3_2_4_3_3_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		