
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// Scan_TO_Scan
class TrCond_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) && 
 worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING 
&& !worldstate.State.GameStateMessage.kickoff()
&& (behavior.State.HeadToBMessage ==NULL 
|| behavior.State.HeadToBMessage.ballfound()==0)
 */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();

		Logger::Instance().WriteMsg("Scan_TO_Scan, TimeoutCheck(behavior) &&   worldstate.State.GameStateMessage.player_state()==PLAYER_PLAYING  && !worldstate.State.GameStateMessage.kickoff() && (behavior.State.HeadToBMessage ==NULL  || behavior.State.HeadToBMessage.ballfound()==0) " ,_toString((msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) &&   (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING)  && (var_1263977940.get()!=0 && !var_1263977940->kickoff() )&& (var_1901744185.get()==0 ||  (var_1901744185.get()!=0 && var_1901744185->ballfound()==0)) ),  Logger::Info);
		return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) &&   (var_1263977940.get()!=0 && var_1263977940->player_state()==PLAYER_PLAYING)  && (var_1263977940.get()!=0 && !var_1263977940->kickoff() )&& (var_1901744185.get()==0 ||  (var_1901744185.get()!=0 && var_1901744185->ballfound()==0))  );
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// Scan_TO_Scan
class TrAction_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.250 */
	public:	TrAction_GoaliePenalty0_2_2_3_3_20_2_2_3_3_2() : statechart_engine::TimeoutAction( "behavior", 250 ) { 
		;
	 }
};

		