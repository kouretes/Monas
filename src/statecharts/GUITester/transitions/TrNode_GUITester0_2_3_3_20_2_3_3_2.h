
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// GoToPositionTOGoToPosition
class TrCond_GUITester0_2_3_3_20_2_3_3_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("motion",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(motion) && behavior.State.GameStateMessage.player_state()==PLAYER_PLAYING && !robotInPosition(behavior.Data.WorldInfo, behavior.State.PositionMessage) */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("motion");

		
		_blk->process_messages();
		if( msg.get() == 0){
			return true;
		}
 		if(msg->wakeup()=="")
			return true;

		if ( var_621149599.get() != 0  && var_1071592760.get() != 0 && var_1868674971.get() != 0 ){	
				return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && !robotInPosition(var_1071592760, var_1868674971) );
		}return false;

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// GoToPositionTOGoToPosition
class TrAction_GUITester0_2_3_3_20_2_3_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.motion.250 */
	public:	TrAction_GUITester0_2_3_3_20_2_3_3_2() : statechart_engine::TimeoutAction( "motion", 250 ) { 
		;
	 }
};

		