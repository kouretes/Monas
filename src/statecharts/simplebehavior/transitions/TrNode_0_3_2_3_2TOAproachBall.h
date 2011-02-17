
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"			
class TrCond_0_3_2_3_2TOAproachBall : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_signal<HeadToBMessage> ("HeadToBMessage");
		if ( obsm == 0 && hbm==0){
				std::cout<<"*************Transition AproachBall FALSE*****************"<<std::endl;
			return false;
		}
		int side;
		float posx=0.14, posy=0.035;
		
		side = (obsm->ball().bearing() > 0) ? 1 : -1;
		
		if(obsm==0 && hbm->ballfound()>0 && gsm->player_state()==PLAYER_PLAYING){
				std::cout<<"*************Transition AproachBall TRUE*****************"<<std::endl;
			return true;
		}
			
		if ( ((obsm!=0 && (fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) > 0.015 || fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) > 0.015)) || hbm->ballfound()>0) && gsm->player_state()==PLAYER_PLAYING){
				std::cout<<"*************Transition AproachBall TRUE*****************"<<std::endl;
				return true;
			}
				std::cout<<"*************Transition AproachBall FALSE*****************"<<std::endl;
		return false;
    }
};
		
