
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"		
#include "messages/Gamecontroller.pb.h"	
class TrCond_0_3_2_3_2TOkickBall : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		 if(gsm==0)
			return false;
	boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");

		if ( obsm == 0 ){
			std::cout<<"*************Transition KickBall FALSE*****************"<<std::endl;
			return false;	
		}
		int side;
		float posx=0.14, posy=0.035;
		
		side = (obsm->ball().bearing() > 0) ? 1 : -1;

		if (( fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) < 0.015 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) < 0.015) && gsm->player_state()==PLAYER_PLAYING) {
				std::cout<<"*************Transition KickBall TRUE*****************"<<std::endl;
			return true;
		}
			std::cout<<"*************Transition KickBall FALSE*****************"<<std::endl;
		return false;
    }
};
		
