
#include "StandUpAndScan.h"

namespace {
    ActivityRegistrar<StandUpAndScan>::Type temp("StandUpAndScan");
}

int StandUpAndScan::Execute() {
	
Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
	//if(robot->isRunning(robot->post.version())){
//		Logger::Instance().WriteMsg(GetName(),  " ALRobotPose is running", Logger::Info);
//		robotpose = robot->getActualPoseAndTime();
//		Logger::Instance().WriteMsg(GetName(),  " robotpose ok", Logger::Info);
	//	if(robotpose[0]!=NULL)
	//		pose = _toString(robotpose[0]);
//	}
	
//		Logger::Instance().WriteMsg(GetName(),  " execute", Logger::Info);
//	Logger::Instance().WriteMsg(GetName(), _toString( robotpose[0] ), Logger::Info);
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	rsp = _blk->readState<RobotStandingPose> ("behavior");
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
		}
		else{
			headaction = SCANFORBALL;
		//	if(pose.compare("Stand")==0)
		//		;
		//	else{
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
		//	}
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
	
	try
	{
		pbroker = AL::ALPtr<AL::ALBroker>(KAlBroker::Instance().GetBroker());
		robot = AL::ALPtr<AL::ALRobotPoseProxy>(new AL::ALRobotPoseProxy(pbroker));
	} catch (AL::ALError& e)
	{
		Logger::Instance().WriteMsg(GetName(), "Error in getting frameManager proxy" + e.getDescription(), Logger::FatalError);
	}
	robotpose = new AL::ALValue();
	pose = "Stand";
}

std::string StandUpAndScan::GetName () {
	return "StandUpAndScan";
}
	
