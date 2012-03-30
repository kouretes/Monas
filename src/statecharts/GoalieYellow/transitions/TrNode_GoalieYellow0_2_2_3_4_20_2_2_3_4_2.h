
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// ApproachBall_TO_ApproachBall
class TrCond_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) && 
 worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.State.HeadToBMessage.ballfound()>0&& !ballAway(worldstate.Data.WorldInfo) && !readyToKick(worldstate.Data.WorldInfo) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1111221333 = _blk->readData<WorldInfo> ("worldstate" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();

		Logger::Instance().WriteMsg("ApproachBall_TO_ApproachBall, TimeoutCheck(behavior) &&   worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.State.HeadToBMessage.ballfound()>0&& !ballAway(worldstate.Data.WorldInfo) && !readyToKick(worldstate.Data.WorldInfo)" ,_toString((msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) &&   (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_1901744185.get()!=0 && var_1901744185->ballfound()>0)&& !ballAway(var_1111221333) && !readyToKick(var_1111221333)),  Logger::Info);
		return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) &&   (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING) && (var_1901744185.get()!=0 && var_1901744185->ballfound()>0)&& !ballAway(var_1111221333) && !readyToKick(var_1111221333) );
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// ApproachBall_TO_ApproachBall
class TrAction_GoalieYellow0_2_2_3_4_20_2_2_3_4_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoalieYellow0_2_2_3_4_20_2_2_3_4_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		