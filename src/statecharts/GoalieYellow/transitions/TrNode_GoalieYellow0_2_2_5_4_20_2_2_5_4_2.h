
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// GoToPosition_TO_GoToPosition
class TrCond_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* TimeoutCheck(behavior) 
&& worldstate.State.GameStateMessage.player_state() == PLAYER_PLAYING 
&& robotInPosition(worldstate.Data.WorldInfo, behavior.State.PositionMessage)
 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const PositionMessage> var_1868674971 = _blk->readState<PositionMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();
	
				return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time())  && (var_1263977940.get()!=0 && var_1263977940->player_state() == PLAYER_PLAYING)  && robotInPosition(var_1111221333, var_1868674971)  );	
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// GoToPosition_TO_GoToPosition
class TrAction_GoalieYellow0_2_2_5_4_20_2_2_5_4_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieYellow0_2_2_5_4_20_2_2_5_4_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		