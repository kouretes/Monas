
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// _open_group_play_or_NoPlay_close_group__TO__open_group_play_or_NoPlay_close_group_
class TrCond_TesterGUI0_3_20_3_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(behavior) && _behavior.State.GameStateMessage.player_state()!=PLAYER_FINISHED */
		
		boost::shared_ptr<const GameStateMessage> var_1692513826 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("behavior");

		
		if ( var_1692513826.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}
		if( msg.get() == 0){
			Logger::Instance().WriteMsg("TIMEOUT", "true",  Logger::Info);
		return true;
		}
 		if(msg->wakeup()=="")
			return true;

		
		Logger::Instance().WriteMsg("boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_1692513826->player_state()!=PLAYER_FINISHED", _toString(boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_1692513826->player_state()!=PLAYER_FINISHED),  Logger::Info);
		return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_1692513826->player_state()!=PLAYER_FINISHED ); 

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// _open_group_play_or_NoPlay_close_group__TO__open_group_play_or_NoPlay_close_group_
class TrAction_TesterGUI0_3_20_3_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.behavior.300 */
	public:	TrAction_TesterGUI0_3_20_3_2() : statechart_engine::TimeoutAction( "behavior", 300 ) { 
		;
	 }
};

		
