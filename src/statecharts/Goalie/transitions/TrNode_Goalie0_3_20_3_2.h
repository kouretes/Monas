
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// _open_group_playing_or_nonPlaying_close_group_TO_open_group_playing_or_nonPlaying_close_group_
class TrCond_Goalie0_3_20_3_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* TimeoutCheck(behavior) && ( behavior.State.GameStateMessage==NULL || behavior.State.GameStateMessage.player_state()!=PLAYER_FINISHED) */
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();
	
				return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) && ( var_621149599.get()==0 ||  (var_621149599.get()!=0 && var_621149599->player_state()!=PLAYER_FINISHED)) );	
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// _open_group_playing_or_nonPlaying_close_group_TO_open_group_playing_or_nonPlaying_close_group_
class TrAction_Goalie0_3_20_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.300 */
	public:	TrAction_Goalie0_3_20_3_2() : statechart_engine::TimeoutAction( "behavior", 300 ) { 
		;
	 }
};

		