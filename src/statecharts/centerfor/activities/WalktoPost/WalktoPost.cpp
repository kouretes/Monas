
#include "WalktoPost.h"
#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<WalktoPost>::Type temp("WalktoPost");
}

int WalktoPost::Execute() {
	
	//Logger::Instance().WriteMsg("WalkToPost",  " Execute", Logger::Info);
	sleep(3);
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	float X = 0.0, Y = 0.0, theta = 0.0;
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	float posx=0.14, posy=0.035;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		//Logger::Instance().WriteMsg("WalkToPost",  " NO OBSM", Logger::Info);
		return 0;
	}
	int side ;//= 1;
	
	bd = obsm->ball().dist();
	bb = obsm->ball().bearing();
	bx = obsm->ball().dist() * cos(obsm->ball().bearing()); //kanw tracking me to swma
	by = obsm->ball().dist() * sin(obsm->ball().bearing());
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
		littleWalk((bx-posx-0.3)*g,(by-offsety-0.3)*g,0,1);
	}
	bhmsg->set_headaction(headaction);
	_blk->publish_signal(*bhmsg, "behavior");
	return 0;
}

void WalktoPost::UserInit () {
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	headaction= BALLTRACK;
}

std::string WalktoPost::GetName () {
	return "WalktoPost";
}
	

void WalktoPost::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void WalktoPost::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}
