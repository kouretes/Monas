
#include "architecture/statechartEngine/ICondition.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/archConfig.h"			
#include "tools/toString.h"
#include "tools/logger.h"
			
class TrCond_AproachBallFor_one_or_more_times_TOkick : public statechart_engine::ICondition {
			
public:

	void UserInit () {
		_blk->subscribeTo("vision", 0);
	 }

	bool Eval() {
		
		//boost::shared_ptr<const ObservationMessage> obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
		
		//if ( obsm.get() == 0 ){
			////Logger::Instance().WriteMsg("TrCond_AproachBallFor_one_or_more_times_TOkick", "FALSE NO OBSM", Logger::Info);
			//return false;
		//}else{
			//int side;
			//float posx=0.117, posy=0.03;
			
			//side = (obsm->ball().bearing() > 0) ? 1 : -1;
			
			//if ((fabs( obsm->ball().dist() * cos(obsm->ball().bearing()) - posx ) <= 0.01 && fabs( obsm->ball().dist() * sin(obsm->ball().bearing()) - (side*posy) ) <= 0.012) ){
				////Logger::Instance().WriteMsg("TrCond_AproachBallFor_one_or_more_times_TOkick", "TRUE", Logger::Info);
				//return true;
			//}else{
			////	Logger::Instance().WriteMsg("TrCond_AproachBallFor_one_or_more_times_TOkick", "FALSE", Logger::Info);
				//return false;
			//}
		//}
		return true;
    }
};
		
