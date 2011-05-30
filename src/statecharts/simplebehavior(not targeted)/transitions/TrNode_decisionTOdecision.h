
#include "architecture/statechartEngine/ICondition.h"
#include "messages/timeout.pb.h"		
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "tools/logger.h"
#include "tools/toString.h"	
#include "architecture/statechartEngine/IAction.h"
#include "architecture/statechartEngine/TimoutAciton.h"	
	
class TrCond_decisionTOdecision : public statechart_engine::ICondition {
			
public:
void UserInit () {
		_blk->subscribeTo("behavior",0); 
		}

	bool Eval() {
		Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " enter", Logger::Info);
		boost::shared_ptr<const TimeoutMsg> msg  = _blk->readState<TimeoutMsg>("behavior");
		std::string time;
		if ( msg.get() != 0 ) {
			time = msg->wakeup();
			int timeout=-1;
			if(time==""){
				Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " empty timeout", Logger::Info);
			return false;
			}
			 timeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(time) ).total_microseconds();
			if ( time != "" && timeout < 0) {
				Logger::Instance().WriteMsg("TrCond_decisionTOdecision", " Timeout not ended", Logger::Info);
				return false;
			}
		}

		Logger::Instance().WriteMsg("TrCond_decisionTOdecision", "TRUE", Logger::Info);
	//	boost::shared_ptr<const TimeoutMsg> tmsg = _blk->read_state<TimeoutMsg> ("TimeoutMsg");
		//std::string stime = tmsg->wakeup();
		//if (tmsg==0 || stime==""){
			//std::cout<<"*************Transition TrCond_decisionTOdecision FALSE  tmsg==0 *****************"<<std::endl;
			//return false;
		//}
		//int statetimeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(stime) ).total_microseconds();
		
		//if(statetimeout>0){
			//std::cout<<"*************Transition TrCond_decisionTOdecision TRUE *****************"<<std::endl;
			//return true;
			//}
		//std::cout<<"*************Transition TrCond_decisionTOdecision FALSE *****************"<<std::endl;
		//return false;
		//Refresh state
		_blk->publish_all();
		//SysCall::_usleep(250000);
		_blk->process_messages();
		/* TRUE */
		return true;

	
    }
	
};
				
class Mitsos : public statechart_engine::TimeoutAction {
			
public:

  Mitsos() : statechart_engine::TimeoutAction("behavior",200) {
    Logger::Instance().WriteMsg("Mitsos", "200 milliseconds", Logger::Info);
  }
};

