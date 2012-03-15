
#include "SitDownAndStare.h"

namespace {
    ActivityRegistrar<SitDownAndStare>::Type temp("SitDownAndStare");
}
int SitDownAndStare::Execute() {
	Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
///	if(robot->post!=NULL && robot->isRunning(robot->post.version())){
//		Logger::Instance().WriteMsg(GetName(),  " ALRobotPose is running", Logger::Info);
//		robotpose = robot->getActualPoseAndTime();
//		Logger::Instance().WriteMsg(GetName(),  " robotpose ok", Logger::Info);
	//	if(robotpose[0]!=NULL)
	//		pose = _toString(robotpose[0]);
//	}

	hbm = _blk->readState<HeadToBMessage> ("behavior");
	rsp = _blk->readState<RobotStandingPose> ("behavior");
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			if(rsp.get()!=0 && rsp->pose()=="Standing" ){
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
	
	
	try
	{
		pbroker = AL::ALPtr<AL::ALBroker>(KAlBroker::Instance().GetBroker());
		robot = AL::ALPtr<AL::ALRobotPoseProxy>(new AL::ALRobotPoseProxy(pbroker));
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg(GetName(), "Error in getting frameManager proxy" + e.getDescription(), Logger::FatalError);
	}
	robotpose = new AL::ALValue();
	pose = "Sit";
}
std::string SitDownAndStare::GetName () {
	return "SitDownAndStare";
}
	
