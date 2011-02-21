
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"	
#include "messages/BehaviorMessages.pb.h"	
			
class TrCond_ScanTOScan : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		 _blk->subscribeTo("behavior", 0);
	 }

	bool Eval() {
		_blk->publish_all();
		SysCall::_usleep(300000);
		_blk->process_messages();
		bool cond;
		
		boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");

		if(gsm.get()!=0  && gsm->player_state()!= PLAYER_PLAYING ){
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", "FALSE NO GSM", Logger::Info);
			return false;
		}else if (hbm.get()==0 ){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", "TRUE NO HBM", Logger::Info);
			return true;
		}else{
			cond  = (hbm->ballfound()==0 );			
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOScanForBall", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
		
