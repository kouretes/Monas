
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"		
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>	
#include "activities/BehaviorConst.h"	
// 0.3.2.2TONoPlay
class TrCond_0_3_2_20_3_2_4 : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);

	}

	bool Eval() {
		/* behavior.State.GameStateMessage.player_state != PLAYER_PLAYING */
		_blk->process_messages();
		boost::shared_ptr<const GameStateMessage> var_621149599 = _blk->readState<GameStateMessage> ("behavior" );
	boost::shared_ptr<const TimeoutMsg> msg  = _blk->readState<TimeoutMsg>("behavior");
		std::string time;
		if ( msg.get() != 0 ) {
			time = msg->wakeup();
			int timeout=-1;
			if(time==""){
			//	Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " empty timeout", Logger::Info);
				//_blk->publish_all();
				//SysCall::_usleep(250000);
			//	_blk->process_messages();
				return true;
			}
			 timeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(time) ).total_microseconds();
			if ( time != "" && timeout < 0) {
				//Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " Timeout not ended", Logger::Info);
				return false;
			}
		}

		if (  var_621149599.get() == 0 ) { return false;}

		
		return ( var_621149599->player_state() != PLAYER_PLAYING ); 

		
    }
};
		
