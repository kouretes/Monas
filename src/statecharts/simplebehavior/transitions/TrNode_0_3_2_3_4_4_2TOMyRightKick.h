
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"		
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_0_3_2_3_4_4_2TOMyRightKick : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
      }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
		if (obsm==0){
			std::cout<<"*************Transition MyRightKick FALSE*****************"<<std::endl;
			return false;
		}
		if ((obsm->ball().dist() * sin(obsm->ball().bearing()) < 0.0)&& gsm->player_state()==PLAYER_PLAYING) {
			std::cout<<"*************Transition MyRightKick TRUE*****************"<<std::endl;
			return true;
		}
		std::cout<<"*************Transition MyRightKick FALSE*****************"<<std::endl;
		return false;
    } 
};
		
