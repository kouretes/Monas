
#include "AproachBallFor.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<AproachBallFor>::Type temp("AproachBallFor");
}

int AproachBallFor::Execute() {

	//Logger::Instance().WriteMsg("AproachBallFOR",  " Execute", Logger::Info);
	
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	float posx=0.117, posy=0.03;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		if (lastObsm==0)
			return 0;
		if (rcvObsm < boost::posix_time::microsec_clock::universal_time() ){
			velocityWalk(0,0,0,1);
			headaction=SCANFORBALL;
			return 0;
		}
	}else{
		lastObsm->CopyFrom(*obsm);
		rcvObsm = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(5);
	}
	int side ;//= 1;
	
	bd = lastObsm->ball().dist();
	bb = lastObsm->ball().bearing();
	bx = lastObsm->ball().dist() * cos(lastObsm->ball().bearing()); //kanw tracking me to swma
	by = lastObsm->ball().dist() * sin(lastObsm->ball().bearing());
	side = (bb > 0) ? 1 : -1;

   
	static float X=0,Y=0,th=0,f=0.2;
	//X=(bx-posx)*2;
	X=(bx-posx )*3;
	X=X>0?X:X-0.01;
	X=X>1?1:X;
	X=X<-1?-1:X;
	//Y=(by-offsety)*1.6;
	float offsety=side*posy;
	Y=(by-offsety)*3;
	if(bd>0.26)
	{
		if(bx<0)
			th=0.2 *Y;
		else
			th=0.1 *Y;

		Y=Y/2.0;

	}
	else
		th=-0.06*by*(Y>0?-1:1);


	Y=Y>0?Y+0.01:Y-0.01;
	Y=Y>1?1:Y;
	Y=Y<-1?-1:Y;
	f=1;

	th=th>1?1:th;
	th=th<-1?-1:th;

    velocityWalk(X,Y,th,f);
	bhmsg->set_headaction(headaction);
	_blk->publish_state(*bhmsg, "behavior");
	return 0;
	}

void AproachBallFor::UserInit () {
	rcvObsm = boost::posix_time::microsec_clock::universal_time();
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	lastObsm = new ObservationMessage();
	headaction= BALLTRACK;
	}

std::string AproachBallFor::GetName () {
	return "AproachBallFor";
}
	

void AproachBallFor::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

void AproachBallFor::littleWalk(double x, double y, double th, int s) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot->set_command("walkTo");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	_blk->publish_signal(*wmot, "motion");
}

