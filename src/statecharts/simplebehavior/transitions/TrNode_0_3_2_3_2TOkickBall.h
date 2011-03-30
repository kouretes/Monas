
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"		
#include "messages/Gamecontroller.pb.h"	
#include "tools/toString.h"
#include "tools/logger.h"

class TrCond_0_3_2_3_2TOkickBall : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
	//	_blk->process_messages();
		//boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
			boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("ObservationMessage");
		//if(gsm.get()==0){
			
		//	return false;
		//}else 
		if ( obsm.get() == 0 ){
			//std::cout<<"*************Transition KickBall FALSE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOkickBall", "FALSE NO OBSM", Logger::Info);
			return false;	
		}else{
			int side;
			float posx=0.117, posy=0.03;
			
			side = (obsm->ball().bearing() > 0) ? 1 : -1;

			if ( fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.01 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <=0.012) {
				//std::cout<<"*************Transition KickBall TRUE*****************"<<std::endl;
				//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOkickBall", "TRUE", Logger::Info);
				return true;
			}else{
				//std::cout<<"*************Transition KickBall FALSE*****************"<<std::endl;
				//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOkickBall", "FALSE", Logger::Info);
				return false;
			}
		}
    }
};
		
