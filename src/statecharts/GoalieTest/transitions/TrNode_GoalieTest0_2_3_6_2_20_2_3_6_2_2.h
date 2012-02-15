
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include "tools/BehaviorConst.h"	
#include "tools/logger.h"
#include "tools/toString.h"
// StareTOStare
class TrCond_GoalieTest0_2_3_6_2_20_2_3_6_2_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior",msgentry::SUBSCRIBE_ON_TOPIC);
		_blk->updateSubscription("vision",msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* TimeoutCheck(vision) && behavior.State.GameStateMessage.player_state()==PLAYER_PLAYING && behavior.State.HeadToBMessage.ballfound()!=0 && ballAway(behavior.Data.WorldInfo) && _behavior.Signal.FallMessage.fall()==0 */
		
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const HeadToBMessage> var_1901744185 = _blk->readState<HeadToBMessage> ("behavior" );
		boost::shared_ptr<const WorldInfo> var_1071592760 = _blk->readData<WorldInfo> ("behavior" );
		boost::shared_ptr<const FallMessage> var_835732259 = _blk->readSignal<FallMessage> ("behavior" );
		boost::shared_ptr<const TimeoutMsg > msg = _blk->readState< TimeoutMsg > ("vision");

		
		_blk->process_messages();
		if( msg.get() == 0){
			Logger::Instance().WriteMsg("TIMEOUT", "true",  Logger::Info);
		return true;
		}
 		if(msg->wakeup()=="")
			return true;
		if ( var_621149599.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_1901744185.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_1071592760.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "false",  Logger::Info);
			return false;
		}
		if ( var_835732259.get() == 0 ){
			Logger::Instance().WriteMsg("Condition", "true",  Logger::Info);
			return true;
		}

		
		Logger::Instance().WriteMsg("boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && var_1901744185->ballfound()!=0 && ballAway(var_1071592760) && var_835732259->fall()==0", _toString(boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && var_1901744185->ballfound()!=0 && ballAway(var_1071592760) && var_835732259->fall()==0),  Logger::Info);
		return ( boost::posix_time::from_iso_string(msg->wakeup())<boost::posix_time::microsec_clock::local_time() && var_621149599->player_state()==PLAYER_PLAYING && var_1901744185->ballfound()!=0 && ballAway(var_1071592760) && var_835732259->fall()==0 ); 

		
    }
};
		
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// StareTOStare
class TrAction_GoalieTest0_2_3_6_2_20_2_3_6_2_2 : public statechart_engine::
				TimeoutAction {
		/* TimeoutAction.vision.250 */
	public:	TrAction_GoalieTest0_2_3_6_2_20_2_3_6_2_2() : statechart_engine::TimeoutAction( "vision", 250 ) { 
		;
	 }
};

		