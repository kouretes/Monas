
#include "Stare.h"

namespace {
    ActivityRegistrar<Stare>::Type temp("Stare");
}

int Stare::Execute() {
	Logger::Instance().WriteMsg("Stare",  " Start", Logger::Info);
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	velocityWalk(0.0, 0.0,0.0,1);
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		//Logger::Instance().WriteMsg("Approachball",  " No OBS", Logger::Info);
		if (lastObsm==0)
			return 0;
	}else{
		lastObsm->CopyFrom(*obsm);
		rcvObsm = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(3);
	}
	int side ;//= 1;
	
	bd = lastObsm->ball().dist();
	bb = lastObsm->ball().bearing();
	bx = lastObsm->ball().dist() * cos(lastObsm->ball().bearing()); //kanw tracking me to swma
	by = lastObsm->ball().dist() * sin(lastObsm->ball().bearing());
	side = (bb > 0) ? 1 : -1;

  
	static float X=0,Y=0,th=0,f=0.2;
	//X=(bx-posx)*2;

	//Y=(by-offsety)*1.6;
	float offsety=side*dDistBallY;
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

	f=1;

	th=th>1?1:th;
	th=th<-1?-1:th;

	velocityWalk(0,0,th,f);
	
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	Logger::Instance().WriteMsg("Stare",  " end", Logger::Info);
	return 0;
}

void Stare::UserInit () {
	rcvObsm = boost::posix_time::microsec_clock::universal_time();
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("sensors", 0);
	_blk->subscribeTo("behavior", 0);

	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	lastObsm = new ObservationMessage();
	headaction= BALLTRACK;
}

std::string Stare::GetName () {
	return "Stare";
}
	

void Stare::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}
