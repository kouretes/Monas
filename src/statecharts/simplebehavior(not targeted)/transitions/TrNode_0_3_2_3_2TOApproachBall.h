
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"		
#include "tools/toString.h"
#include "tools/logger.h"

class TrCond_0_3_2_3_2TOApproachBall : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		//_blk->process_messages();
		//boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage> ("ObservationMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage> ("HeadToBMessage");
		if ( hbm.get()==0){
			//std::cout<<"*************Transition AproachBall FALSE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "FALSE NO OBSM, HBM", Logger::Info);
			return false;
		}else{
			int side;
			float posx=0.117, posy=0.03;
			if(obsm.get()!=0)
				side = (obsm->ball().bearing() > 0) ? 1 : -1;
			
			if( hbm->ballfound()==0){// && hbm->ballfound()>0 && gsm->player_state()==PLAYER_PLAYING){
				//std::cout<<"*************Transition AproachBall TRUE*****************"<<std::endl;
			//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "FALSE NO OBSM", Logger::Info);
				return false;
			}else if(hbm->ballfound()>0 && obsm.get()==0){
			//	SysCall::_usleep(50000);
			//	_blk->process_messages();
				return true;
				
			}else if ( obsm.get()!=0 && (fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) > 0.01 || fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) > 0.012) ){//&& gsm->player_state()==PLAYER_PLAYING){
				//std::cout<<"*************Transition AproachBall TRUE*****************"<<std::endl;
				//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "TRUE", Logger::Info);
				return true;
			}else{
				//std::cout<<"*************Transition AproachBall FALSE*****************"<<std::endl;
			//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOAproachBall", "FALSE", Logger::Info);
				return false;
			}
		}
    }
};
		
