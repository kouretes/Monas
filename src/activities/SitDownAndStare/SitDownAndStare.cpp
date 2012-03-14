
#include "SitDownAndStare.h"

namespace {
    ActivityRegistrar<SitDownAndStare>::Type temp("SitDownAndStare");
}
int SitDownAndStare::Execute() {
	
	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);

	hbm = _blk->readState<HeadToBMessage> ("behavior");
	rsp = _blk->readState<RobotStandingPose> ("behavior");
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			if(rsp.get()!=0 && rsp->pose()=="Standing"){
				amot->set_command("SitDownX.xar");
				_blk->publishSignal(*amot, "motion");
				rsp_msg->set_pose("Sitting");
				_blk->publishState(*rsp_msg, "behavior");
				Logger::Instance().WriteMsg(GetName(),  " PUBLISH SIT", Logger::Info);
			}
		}
		else{
			headaction = SCANFORBALL;
		}
	}

	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");

	_blk->publish_all();
	return 0;
}
void SitDownAndStare::UserInit () {	
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	amot=  new MotionActionMessage();
	rsp_msg = new RobotStandingPose();
}
std::string SitDownAndStare::GetName () {
	return "SitDownAndStare";
}
	
