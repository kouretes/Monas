
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"
			
class TrCond_decision_one_or_more_times_TO0_4 : public statechart_engine::ICondition {
			
public:


	void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		 //boost::shared_ptr<const GameStateMessage> gsm ;
		 //gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
     //if ( gsm == 0 ){
		 std::cout<<"*************Transition TrCond_decision_one_or_more_times_TO0_4 FALSE*****************"<<std::endl;
		 //return false;
		 //}
     //if( gsm->player_state()!=PLAYER_FINISHED){
		//std::cout<<"*************Transition TrCond_decision_one_or_more_times_TO0_4 FALSE*****************"<<std::endl;
        //return false;
	//}
	 //std::cout<<"*************Transition TrCond_decision_one_or_more_times_TO0_4 TRUE*****************"<<std::endl;
	 //return true;
	 return false;
    }
};
		
