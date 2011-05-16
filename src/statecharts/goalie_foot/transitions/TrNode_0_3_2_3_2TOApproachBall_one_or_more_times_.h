
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"			
#include "activities/ApproachBall/ApproachBall.h"			
#include "activities/Stare/Stare.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_0_3_2_3_2TOApproachBall_one_or_more_times_ : public statechart_engine::ICondition {
			
public:
	/* ballfound!=0 and !ballaway and !readyTokick and toFallOrNotToFall==0 */
	void UserInit () {
		_blk->subscribeTo("behavior", 0);
		_blk->subscribeTo("vision", 0);
	}

	bool Eval() {
		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_2TOApproachBall_one_or_more_times_", "FALSE NO OBSM", Logger::Info);
		boost::shared_ptr<const ObservationMessage> obsm = _blk->readSignal<ObservationMessage>("vision");
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const DoubleObsInfo> doi = _blk->readData<DoubleObsInfo>("behavior");
		Stare st;
		ApproachBall ab;
		if(hbm.get()!=0 && hbm->ballfound()!=0){
			if(obsm.get()!=0 && !ab.ballAway(obsm) && !ab.readyToKick(obsm)){
				if(doi.get()!=0 && st.toFallOrNotToFall(doi)==0)
					return true;
			}
		}
		return false;
    }
};
	
