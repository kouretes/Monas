
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"	
#include "messages/BehaviorMessages.pb.h"	
		
class TrCond_0_3_2_3_2TOScanForBall : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		bool cond;
		//_blk->process_messages();
		//boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
		//boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
		
	//	if(gsm.get()==0){
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", "FALSE NO GSM", Logger::Info);
		//	return false;
		//}else 
		if (hbm.get()==0){
			//std::cout<<"*************Transition ScanForBall FALSE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", "TRUE NO HBM", Logger::Info);
			return true;
		}else{
			cond  = (hbm->ballfound()==0 ); //&& gsm->player_state()==PLAYER_PLAYING);
			//std::cout<<"*************Transition ScanForBall"<<(hbm->ballfound()==0)<<"*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
