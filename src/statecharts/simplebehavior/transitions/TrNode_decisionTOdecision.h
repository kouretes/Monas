
#include "architecture/statechartEngine/ICondition.h"
#include "messages/timeout.pb.h"		
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
	
class TrCond_decisionTOdecision : public statechart_engine::ICondition {
			
public:
void UserInit () {
		_blk->subscribeTo("behavior",0); 
		}

	bool Eval() {
		
		boost::shared_ptr<const TimeoutMsg> tmsg = _blk->read_state<TimeoutMsg> ("TimeoutMsg");
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
		/* TRUE */
		return true;

	
    }
	
};
		
