
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"
			
class TrCond_0_3_2_2TOplay : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
     if ( gsm == 0 ){
		 std::cout<<"*************Transition Play FALSE*****************"<<std::endl;
		 return false;
		 }
     if( gsm->player_state()!=PLAYER_PLAYING){
		std::cout<<"*************Transition Play FALSE*****************"<<std::endl;
        return false;
	}
	 std::cout<<"*************Transition Play TRUE*****************"<<std::endl;
	 return true;
    }
};
		
