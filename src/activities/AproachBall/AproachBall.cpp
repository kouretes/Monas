
#include "AproachBall.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace {
    ActivityRegistrar<AproachBall>::Type temp("AproachBall");
}

int AproachBall::Execute() {
//gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	//if(gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE APROACHBALL NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	std::cout << "STATE APROACHBALL" <<std::endl;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::local_time()+boost::posix_time::millisec(2000);
	tmsg->set_wakeup(boost::posix_time::to_iso_string(timeout));
	_blk->publish_state(*tmsg, "behavior");
	lastObsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	float X = 0.0, Y = 0.0, theta = 0.0;
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	float posx=0.14, posy=0.035;
	
	headaction = BALLTRACK;
	if (lastObsm==0){
		std::cout << "STATE APROACHBALL NOOBSM" <<std::endl;
		return 0;
	}
	int side ;//= 1;
	
	bd = lastObsm->ball().dist();
	bb = lastObsm->ball().bearing();
	bx = lastObsm->ball().dist() * cos(lastObsm->ball().bearing()); //kanw tracking me to swma
	by = lastObsm->ball().dist() * sin(lastObsm->ball().bearing());
	side = (bb > 0) ? 1 : -1;

   if(bd>0.5){
		float X=0,Y=0,th=0;
		if(fabs(bx)>0.15) X=1;
		if(fabs(by)>0.15) Y=0.3*side;
		velocityWalk(X,Y,0.2*bb,1);
	}
	else
	{
		float offsety=side*posy;
		float g=0.3;
		littleWalk((bx-posx)*g,(by-offsety)*g,0,1);
	}
	bhmsg->set_headaction(headaction);
	_blk->publish_signal(*bhmsg, "behavior");
	return 0;
}

void AproachBall::UserInit () {
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	tmsg = new TimeoutMsg();
	headaction= BALLTRACK;
}

std::string AproachBall::GetName () {
	return "AproachBall";
}
	

void AproachBall::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void AproachBall::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}
