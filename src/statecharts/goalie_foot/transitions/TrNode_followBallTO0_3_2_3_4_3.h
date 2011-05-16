
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "activities/Stare/Stare.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_followBallTO0_3_2_3_4_3 : public statechart_engine::ICondition {
			
public:

	void UserInit () { 
		_blk->subscribeTo("behavior",0);
		_blk->subscribeTo("vision",0);
		}

	bool Eval() {
		/* ballfound==0 or readyTokick or (!ballaway and toFallOrNotTOFall==0) */
		//Logger::Instance().WriteMsg("TrCond_StareTOStare", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");

		ApproachBall ab;
		Stare st;
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		if(gsm.get()!=0 && gsm->player_state()==PLAYER_PLAYING)
			return true;
		if(hbm.get()!=0 && hbm->ballfound()==0){
		//	if(obsm.get()!=0 && ab.ballAway(obsm))
				return true;
		}
		if( obsm.get()!=0 && ab.readyToKick(obsm) ){
			return true;
		}
		if(obsm.get()!=0 && !ab.ballAway(obsm) && doi.get()!=0 && st.toFallOrNotToFall(doi)==0)
			return true;
		return false;
	}
};
