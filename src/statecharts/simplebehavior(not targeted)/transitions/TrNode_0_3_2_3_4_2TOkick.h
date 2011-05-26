
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_4_2TOkick : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
    }

	bool Eval() {
		bool cond;
		//_blk->process_messages();
		boost::shared_ptr<const KickOffMessage> kcm = _blk->readState<KickOffMessage> ("behavior");
	//	boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		//if(gsm==0){
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKick", "FALSE", Logger::Info);
	//		return false;
	//	}else 
	if (kcm==0 ){//&& gsm->player_state()==PLAYER_PLAYING){
			//std::cout<<"*************Transition Kick TRUE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKick", "TRUE", Logger::Info);
			return true;
		}else{
			cond = (!kcm->kickoff() );// && gsm->player_state()==PLAYER_PLAYING);
			//std::cout<<"*************Transition KickOff"<<(!kcm->kickoff())<<"*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKick", _toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
