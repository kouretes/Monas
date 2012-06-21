
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// 0.2.2_TO_0.2.2
class TrCond_GoaliePenalty0_2_20_2_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* TimeoutCheck(behavior) && ( worldstate.State.GameStateMessage==NULL || worldstate.State.GameStateMessage.player_state()!=PLAYER_FINISHED) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();
	
				return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) && ( var_1263977940.get()==0 ||  (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_FINISHED)) );	
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// 0.2.2_TO_0.2.2
class TrAction_GoaliePenalty0_2_20_2_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.300 */
	public:	TrAction_GoaliePenalty0_2_20_2_2() : statechart_engine::TimeoutAction( "behavior", 300 ) { 
		;
	 }
};

		