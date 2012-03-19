
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// _open_group_play_or_NoPlay_close_group_TO_open_group_play_or_NoPlay_close_group_
class TrCond_GUITester0_20_2 : public statechart_engine::ICondition {		
public:
	void UserInit () {
		_blk->updateSubscription("worldstate",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) && (worldstate.State.GameStateMessage==NULL ||worldstate.State.GameStateMessage.player_state()!=PLAYER_FINISHED) */
		boost::shared_ptr<const GameStateMessage> var_1263977940 = _blk->readState<GameStateMessage> ("worldstate" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");
		_blk->process_messages();

		Logger::Instance().WriteMsg("_open_group_play_or_NoPlay_close_group_TO_open_group_play_or_NoPlay_close_group_, TimeoutCheck(behavior) && (worldstate.State.GameStateMessage==NULL ||worldstate.State.GameStateMessage.player_state()!=PLAYER_FINISHED)" ,_toString((msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) && (var_1263977940.get()==0 || (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_FINISHED))),  Logger::Info);
		return ( (msg.get()!=0 && msg->wakeup()!="" && boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time()) && (var_1263977940.get()==0 || (var_1263977940.get()!=0 && var_1263977940->player_state()!=PLAYER_FINISHED)) );
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// _open_group_play_or_NoPlay_close_group_TO_open_group_play_or_NoPlay_close_group_
class TrAction_GUITester0_20_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.350 */
	public:	TrAction_GUITester0_20_2() : statechart_engine::TimeoutAction( "behavior", 350 ) { 
		;
	 }
};

		