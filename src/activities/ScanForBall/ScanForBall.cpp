
#include "ScanForBall.h"
#include "messages/RoboCupGameControlData.h"
namespace {
    ActivityRegistrar<ScanForBall>::Type temp("ScanForBall");
}

int ScanForBall::Execute() {
	//gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	//if(gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE SCANFORBALL NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	std::cout << "STATE SCANFORBALL" <<std::endl;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::local_time()+boost::posix_time::millisec(600);
	tmsg->set_wakeup(boost::posix_time::to_iso_string(timeout));
	_blk->publish_state(*tmsg, "behavior");
	hbm = _blk->read_state<HeadToBMessage> ("HeadToBMessage");
	scm = _blk->read_signal<ScanMessage> ("ScanMessage");
	
	if( hbm!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			std::cout << "STATE SCANFORBALL publish balltrack" <<std::endl;
		}
		else{		
			if(scm!=0 && scm->scancompleted())
				littleWalk(0.0, 0.0, 45 * TO_RAD, 5);
			else
				velocityWalk(0.0, 0.0, 0.0, 1);			
			headaction = SCANFORBALL;
			std::cout << "STATE SCANFORBALL publish scanforball" <<std::endl;
		}
	}
	bhmsg->set_headaction(headaction);
	_blk->publish_signal(*bhmsg, "behavior");
	return 0;
}

void ScanForBall::UserInit () {
	_blk->subscribeTo("behavior", 0);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	tmsg = new TimeoutMsg();
}

std::string ScanForBall::GetName () {
	return "ScanForBall";
}
	
void ScanForBall::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void ScanForBall::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}


	
