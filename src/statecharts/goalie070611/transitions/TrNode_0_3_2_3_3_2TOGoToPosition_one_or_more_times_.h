
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/BehaviorConst.h"


class TrCond_0_3_2_3_3_2TOGoToPosition_one_or_more_times_ : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
	//	return false;
		/* returnToPosition */
		boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");
		boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");
		boost::shared_ptr<const ReturnToPositionMessage> rpm = _blk->readSignal<ReturnToPositionMessage>("behavior");

		bool inPos =  false;
		//if(wimsg!=0){
			//int	side =1;
			//if(pm!=0 && pm->posx()<0.0)
				//side =-1;
			//if(fabs(wimsg->myposition().y())> 1.15)
				//if(side*wimsg->myposition().x() >2.4 && side*wimsg->myposition().x() <3.1 )
					//inPos = true;
		//}
		if((rpm!=0 && rpm->goalietopos() )){ // && !inPos){
			return true;
		}

		//Logger::Instance().WriteMsg("TrCond_0_3_2_3_3_2TOGoToPosition_one_or_more_times_", " false ", Logger::Info);
		return false;
    }
};

