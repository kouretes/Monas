
#include "ApproachBall.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>


namespace {
    ActivityRegistrar<ApproachBall>::Type temp("ApproachBall");
}

int ApproachBall::Execute() {

	//Logger::Instance().WriteMsg("ApproachBall",  " Execute", Logger::Info);
	
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		//Logger::Instance().WriteMsg("Approachball",  " No OBS", Logger::Info);
		if (lastObsm==0)
			return 0;
		if (rcvObsm < boost::posix_time::microsec_clock::universal_time() ){
			velocityWalk(0,0,0,1);
			headaction=SCANFORBALL;
			return 0;
		}
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
	X=(bx-dDistBallX )*3;
	X=X>0?X:X-0.01;
	X=X>1?1:X;
	X=X<-1?-1:X;
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


	Y=Y>0?Y+0.01:Y-0.01;
	Y=Y>1?1:Y;
	Y=Y<-1?-1:Y;
	f=1;

	th=th>1?1:th;
	th=th<-1?-1:th;

    velocityWalk(X,Y,th,f);
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	
	return 0;
}

void ApproachBall::UserInit () {
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

std::string ApproachBall::GetName () {
	return "ApproachBall";
}
	

void ApproachBall::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void ApproachBall::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}
bool ApproachBall::readyToKick(boost::shared_ptr<const ObservationMessage>  msg){
		if (msg.get()!=0){
			int side;
						
			side = (msg->ball().bearing() > 0) ? 1 : -1;
			if ((fabs( msg->ball().dist() * cos(msg->ball().bearing()) - dDistBallX ) <= dDistBallOffset && fabs( msg->ball().dist() * sin(msg->ball().bearing()) - (side*dDistBallY) ) <= dDistBallY) ){				
				Logger::Instance().WriteMsg("TrCond_ApproachBall_one_or_more_times_TO0_3_2_3_4_4_2_4", "TRUE", Logger::Info);
				return true;
			}
		}
		return false;

}
float ApproachBall::rotation(float a, float b, float theta){
	
	return a*cos(theta) + b*sin(theta);
	
	
}

float ApproachBall::distance(float x1, float x2, float y1, float y2){
	
	float dis;
	dis = sqrt(pow(x2-x1, 2)+ pow(y2-y1, 2));
	
	return dis;
}
