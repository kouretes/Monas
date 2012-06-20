
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// LocalizeTOLocalize
class TrCond_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* TimeoutCheck(behavior) && worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && (behavior.State.LocalizeDone==NULL || !behavior.State.LocalizeDone.moveon()) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const LocalizeDone> var_373152964 = _blk->readState<LocalizeDone> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();
	
				return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) && (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_373152964.get()==0 ||  (var_373152964.get()!=0 && !var_373152964->moveon())) );	
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// LocalizeTOLocalize
class TrAction_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieRoboCup20120_2_4_2_3_20_2_4_2_3_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		