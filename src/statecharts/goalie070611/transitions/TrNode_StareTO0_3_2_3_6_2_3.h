
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/ApproachBall/ApproachBall.h"
			
class TrCond_StareTO0_3_2_3_6_2_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior", 0);
		}

	bool Eval() {
		/* ballfound==0 || fall || !ballAway */
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOfollowBall_one_or_more_times_", "  ", Logger::Info);
	//	boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg  = _blk->readData<WorldInfo>("behavior");
		boost::shared_ptr<const FallMessage> fm  = _blk->readSignal<FallMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm  = _blk->readState<GameStateMessage>("behavior");
		ApproachBall ab;
		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			Logger::Instance().WriteMsg("TrCond_StareTO0_3_2_3_6_2_3", " !PLAYING true ", Logger::Info);
			return true;
		}
		if(hbm.get()!=0 && hbm->ballfound()==0){
			Logger::Instance().WriteMsg("TrCond_StareTO0_3_2_3_6_2_3", " ballfound==0 true ", Logger::Info);
			return true;
		}
		if(wimsg.get()!=0  && wimsg->balls_size()!=0 ){
			if( !ab.ballAway(wimsg)) {//ball is far away from the goalpost
				Logger::Instance().WriteMsg("TrCond_StareTO0_3_2_3_6_2_3", " !ballAway true ", Logger::Info);
				return true;
			}
		}
		
		if(fm.get()!=0 && fm->fall()!=0){
			Logger::Instance().WriteMsg("TrCond_StareTO0_3_2_3_6_2_3", " fall true ", Logger::Info);
			return true;
		}
		return false;
    }
};
		