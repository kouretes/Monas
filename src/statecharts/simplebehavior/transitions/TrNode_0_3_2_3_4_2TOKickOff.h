
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"		
#include "messages/Gamecontroller.pb.h"				
#include "messages/BehaviorMessages.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"
		
class TrCond_0_3_2_3_4_2TOKickOff : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		 _blk->subscribeTo("behavior", 0);
		  }

	bool Eval() {
		bool cond;
		//_blk->process_messages();
		//boost::shared_ptr<const GameStateMessage> gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const KickOffMessage> kcm = _blk->read_state<KickOffMessage> ("KickOffMessage");
		KickOffMessage* kkm =  new KickOffMessage();
		//if(gsm.get()==0){
		//	Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKickOff", "FALSE NO GSM", Logger::Info);
		//	return false;
	//	}else 
		if (kcm.get()==0){
			//std::cout<<"*************Transition KickOff FALSE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKickOff", "FALSE NO KCM", Logger::Info);
			return false;
		}else{
			cond = kcm->kickoff() ;// && gsm->player_state()==PLAYER_PLAYING);
			//std::cout<<"*************Transition KickOff"<<kcm->kickoff()<<"*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_2TOKickOff",_toString(cond), Logger::Info);
			return cond;
		}
    }
};
		
