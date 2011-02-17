
#include "architecture/statechartEngine/ICondition.h"
#include "messages/timeout.pb.h"				
class TrCond_kickTO0_3_2_3_4_5 : public statechart_engine::ICondition {
			
public:
void UserInit () {
		//_blk->subscribeTo("behavior",0); 
		}

	bool Eval() {
		//std::cout<<"*************Transition TrCond_kickTO0_3_2_3_4_5 TRUE*****************"<<std::endl;
		//boost::shared_ptr<const TimeoutMsg> tmsg = _blk->read_state<TimeoutMsg> ("TimeoutMsg");
		//std::string stime = msg->wakeup();
		//if (tmsg==0 || time=="")
			//return false;
		//int statetimeout = (boost::posix_time::microsec_clock::local_time() - boost::posix_time::from_iso_string(stime) ).total_microseconds();
		
		//if(statetimeout>0)
			//return true;
			
		//return false;
		return true;
		///* TRUE */
    }
};
		
