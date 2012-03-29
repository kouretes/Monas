
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
// Stare_TO_Stare
class TrCond_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}
	bool Eval() {
		/* TimeoutCheck(behavior) &&  
worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.State.HeadToBMessage.ballfound()>0 && ballAway(worldstate.Data.WorldInfo) &&(behavior.State.FallMessage==NULL|| behavior.State.FallMessage.fall()==0) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const FallMessage> var_1561961811 = _blk->readState<FallMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();
	
				return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) &&   (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_1901744185.get()!=0 && var_1901744185->ballfound()>0) && ballAway(var_1111221333) &&(var_1561961811.get()==0 ||  (var_1561961811.get()!=0 && var_1561961811->fall()==0)) );	
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// Stare_TO_Stare
class TrAction_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieYellow0_2_2_3_6_2_20_2_2_3_6_2_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		