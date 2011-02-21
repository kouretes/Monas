
#include "SearchBall.h"
#include "messages/RoboCupGameControlData.h"

#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<SearchBall>::Type temp("SearchBall");
}

int SearchBall::Execute() {
	
	//Logger::Instance().WriteMsg("ScanForBall",  " Execute", Logger::Info);
	hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
	
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			//Logger::Instance().WriteMsg("ScanForBall",  " BALLTRACK", Logger::Info);
		}
		else{		
			
			if (lastTurn< boost::posix_time::microsec_clock::universal_time()){
				littleWalk(0.0, 0.0, 45 * TO_RAD, 5);
				lastTurn = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(6);
			}else{
				velocityWalk(0.0, 0.0, 0.0, 1);		
			}
				
			headaction = SCANFORBALL;			
			//Logger::Instance().WriteMsg("ScanForBall",  " SCANFORBALL", Logger::Info);
		}
	}
	bhmsg->set_headaction(headaction);
	_blk->publish_signal(*bhmsg, "behavior");
	
	return 0;
}

void SearchBall::UserInit () {
	_blk->subscribeTo("behavior", 0);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	lastTurn = boost::posix_time::microsec_clock::universal_time();

}

std::string SearchBall::GetName () {
	return "SearchBall";
}
	
void SearchBall::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void SearchBall::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}
