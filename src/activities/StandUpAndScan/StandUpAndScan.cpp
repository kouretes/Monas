
#include "StandUpAndScan.h"

namespace {
    ActivityRegistrar<StandUpAndScan>::Type temp("StandUpAndScan");
}

int StandUpAndScan::Execute() {
	
	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
	
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	rsp = _blk->readState<RobotStandingPose> ("behavior");
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
		}
		else{
			headaction = SCANFORBALL;

				if(rsp.get()!=0 && rsp->pose()=="Sitting"  ){
					Logger::Instance().WriteMsg(GetName(),  " PUBLISH STAND", Logger::Info);
					amot->set_command("StandUpX.xar");
					_blk->publishSignal(*amot, "motion");
					rsp_msg->set_pose("Standing");
					_blk->publishState(*rsp_msg, "behavior");
				}else{
					rsp_msg->set_pose("Standing");
					_blk->publishState(*rsp_msg, "behavior");
					Logger::Instance().WriteMsg(GetName(),  " Standing", Logger::Info);
				}
		}
	}

	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");

	_blk->publish_all();
	return 0;
}
void StandUpAndScan::UserInit () {	
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	amot=  new MotionActionMessage();
	rsp_msg = new RobotStandingPose();
}

std::string StandUpAndScan::GetName () {
	return "StandUpAndScan";
}
	
