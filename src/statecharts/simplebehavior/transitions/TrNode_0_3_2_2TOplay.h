
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"
#include "messages/RoboCupGameControlData.h"	
#include "messages/Gamecontroller.pb.h"
#include "tools/toString.h"
#include "tools/logger.h"
		
class TrCond_0_3_2_2TOplay : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("behavior", 0);
 }

	bool Eval() {
		//_blk->process_messages();
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage> ("GameStateMessage");
		
		if ( gsm.get() == 0 ){
			//std::cout<<"*************Transition Play FALSE NO GSM*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_2TOplay", "FALSE NO GSM", Logger::Info);
			return false;
		}else if( gsm->player_state()!=PLAYER_PLAYING){
			//std::cout<<"*************Transition Play FALSE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_2TOplay", "FALSE !PLAYING", Logger::Info);
			return false;
		}else{
			//std::cout<<"*************Transition Play TRUE*****************"<<std::endl;
			//Logger::Instance().WriteMsg("TrCond_0_3_2_2TOplay", "TRUE", Logger::Info);
			return true;
		}
    }
};
		
