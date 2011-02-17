
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
			
class TrCond_0_3_2_3_2TOScanForBall : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
		 }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_signal<HeadToBMessage> ("HeadToBMessage");
		if (hbm==0){
				std::cout<<"*************Transition ScanForBall FALSE*****************"<<std::endl;
			return false;
		}
			std::cout<<"*************Transition ScanForBall"<<(hbm->ballfound()==0)<<"*****************"<<std::endl;
		return (hbm->ballfound()==0 && gsm->player_state()==PLAYER_PLAYING);
    }
};
		
