
#include "architecture/statechartEngine/ICondition.h"
#include "messages/AllMessagesHeader.h"
#include "tools/toString.h"
#include "tools/logger.h"
#include "activities/BehaviorConst.h"
#include "activities/GoToPosition/GoToPosition.h"


class TrCond_ScanTOScan : public statechart_engine::ICondition {

public:

	void UserInit () {
		_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	}

	bool Eval() {
		/* ballfound==0  and !ReturnToPosition */
		_blk->publish_all();
		SysCall::_usleep(200000);
		_blk->process_messages();
	//	Logger::Instance().WriteMsg("TrCond_ScanTOScan", " ", Logger::Info);
		boost::shared_ptr<const HeadToBMessage> hbm = _blk->readState<HeadToBMessage>("behavior");
		boost::shared_ptr<const GameStateMessage> gsm = _blk->readState<GameStateMessage>("behavior");
		//boost::shared_ptr<const WorldInfo> wimsg = _blk->readData<WorldInfo>("behavior");
		//boost::shared_ptr<const PositionMessage> pm = _blk->readState<PositionMessage>("behavior");

		if(gsm.get()!=0 && gsm->player_state()!=PLAYER_PLAYING){
			//	Logger::Instance().WriteMsg("TrCond_ScanTOScan", "!PLAYER_PLAYING ", Logger::Info);
				return false;
		}
		if(hbm.get()!=0 && hbm->ballfound()==0){
		//	Logger::Instance().WriteMsg("TrCond_ScanTOScan", "ballfound==0  ", Logger::Info);

			//if(wimsg!=0 ){
				//Logger::Instance().WriteMsg("TrCond_ScanTOScan", "Confidence " +_toString(wimsg->myposition().confidence()), Logger::Info);
				//if( wimsg->myposition().confidence()<badConfidence){
					//return true;
				//}else{
					//GoToPosition g;
					//if(pm!=0 && g.robotInPosition(wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(), pm->posy(), wimsg->myposition().phi(), pm->theta() ))
						//return true;
					//return false;
				//}
			//}

			return true;

		}
		return false;
    }
};

