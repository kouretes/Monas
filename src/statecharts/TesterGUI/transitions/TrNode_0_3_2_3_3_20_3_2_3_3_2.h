
#include "architecture/statechartEngine/ICondition.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>	
#include "messages/AllMessagesHeader.h"			
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"
// GoToPositionTOGoToPosition
class TrCond_0_3_2_3_3_20_3_2_3_3_2 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage.player_state==PLAYER_PLAYING && behavior.Signal.RobotInPosition.inposition==false */
		_blk->process_messages();
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
		boost::shared_ptr<const RobotInPosition> var_529308494 = _blk->readSignal<RobotInPosition> ("behavior" );
		boost::shared_ptr<const TimeoutMsg> msg  = _blk->readState<TimeoutMsg>("behavior");
		std::string time;
		if ( msg.get() != 0 ) {
			time = msg->wakeup();
			int timeout=-1;
			if(time==""){
			//Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " empty timeout", Logger::Info);
				//_blk->publish_all();
				//SysCall::_usleep(250000);
			//	_blk->process_messages();
				return true;
			}
			 timeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(time) ).total_microseconds();
			if ( time != "" && timeout < 0) {
			//	Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " Timeout not ended", Logger::Info);
				return false;
			}
		}

		if (  var_621149599.get() == 0 
		 || var_529308494.get() == 0 ) { return false;}

		
		return ( var_621149599->player_state()==PLAYER_PLAYING && var_529308494->inposition()==false ); 

		
    }
};
		

// GoToPositionTOGoToPosition
class TrAction_0_3_2_3_3_20_3_2_3_3_2 : public statechart_engine::TimeoutAction {
			
public:

  TrAction_0_3_2_3_3_20_3_2_3_3_2() : statechart_engine::TimeoutAction("behavior",300) {
    ;
  }
};
		
