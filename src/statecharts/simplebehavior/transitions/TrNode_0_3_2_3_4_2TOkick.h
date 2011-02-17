
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"	
			
class TrCond_0_3_2_3_4_2TOkick : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		 _blk->subscribeTo("behavior", 0);
      _blk->subscribeTo("vision", 0);
      }

	bool Eval() {
		 boost::shared_ptr<const GameStateMessage> gsm ;
		 gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
		boost::shared_ptr<const KickOffMessage> kcm = _blk->read_signal<KickOffMessage> ("KickOffMessage");
		if (kcm==0 && gsm->player_state()==PLAYER_PLAYING){
				std::cout<<"*************Transition Kick TRUE*****************"<<std::endl;
			return true;
		}
			std::cout<<"*************Transition KickOff"<<(!kcm->kickoff())<<"*****************"<<std::endl;
		return (!kcm->kickoff() && gsm->player_state()==PLAYER_PLAYING);
    }
};
		
