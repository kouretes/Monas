
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "activities/Stare/Stare.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_followBallTOfollowBall : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
		}

	bool Eval() {

		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
		
		/* !readyToKick and ballfound!=0 and  (ballaway or (!ballaway and toFallOrNotToFall!=0) */
		
		
		Logger::Instance().WriteMsg("TrCond_followBallTOfollowBall", " ", Logger::Info);
		//boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
	
		ApproachBall ab;
		//Stare st;
		//boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING)
			return false;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(wimsg.get()!=0 && wimsg->balls_size()!=0 ){
				if(!ab.readyToKick(wimsg)){
					if( ab.ballAway(wimsg)) //ball is far away from the goalpost
						return true;
					//ball is not far away from the goalpost but goalkeeper can repulse the ball	
				//	if(!ab.ballAway(obsm) && doi.get()!=0 && st.toFallOrNotToFall(doi)!=0 )
				//		return true;
				}
			}			
		}
		return false;
	}
};
	
