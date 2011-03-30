
#include "TargetedApproach.h"



namespace {
    ActivityRegistrar<TargetedApproach>::Type temp("TargetedApproach");
}

int TargetedApproach::Execute() {

	//Logger::Instance().WriteMsg("TargetedApproach",  " Execute", Logger::Info);
	headaction = BALLTRACK;
	gsm = _blk->readState<GameStateMessage> ("behavior");
	wimsg = _blk->readData<WorldInfo> ("behavior");
	
	if(gsm.get()!=0 && gsm->own_goal_color()==GOAL_BLUE){
		postX = yellowPostCenterX;
		postY = yellowPostCenterY;
	}	
	else if(gsm.get()!=0 && gsm->team_color()==GOAL_YELLOW){
		postX = -yellowPostCenterX;
		postY = yellowPostCenterY;
	}
		

	if (wimsg.get()==0){
		//Logger::Instance().WriteMsg("TargetedApproach",  " No OBS", Logger::Info);
		if (lastObs==0)
			return 0;
		if (rcvObs < boost::posix_time::microsec_clock::universal_time() ){
			velocityWalk(0,0,0,1);
			headaction=SCANFORBALL;
			return 0;
		}
	}else{
		lastObs->CopyFrom(*wimsg);
		rcvObs = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(3);
		if(wimsg->has_myposition()){
			myPosX = wimsg->myposition().x();
			myPosY = wimsg->myposition().y();
			myPhi = wimsg->myposition().phi();
		}
		if(wimsg->balls_size()>0){
			ballX = ab.rotation(wimsg->balls(0).relativex(), wimsg->balls(0).relativey(), myPhi) + myPosX;
			ballY = ab.rotation( wimsg->balls(0).relativey(), wimsg->balls(0).relativex(), myPhi) + myPosY;
		}
		theta = calculateAngle(ballX, ballY, myPosX, myPosY, myPhi);

		int side ;//= 1;
		side = (theta > 0) ? 1 : -1;
		   
		static float X=0,Y=0,f=0.2, bd=0.0;

		X=(wimsg->balls(0).relativex()-dDistBallX )*3;
		X=X>0?X:X-0.01;
		X=X>1?1:X;
		X=X<-1?-1:X;
		//bd = ab.distance(wimsg->balls(0).relativex(), 0.0,  wimsg->balls(0).relativey(), 0.0);

		float offsety=side*dDistBallY;
		Y=(wimsg->balls(0).relativey()-offsety)*3;
		
		Y=Y>0?Y+0.01:Y-0.01;
		Y=Y>1?1:Y;
		Y=Y<-1?-1:Y;
		f=1;

		velocityWalk(X,Y,theta,f);
    }
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	
	return 0;
}

void TargetedApproach::UserInit () {
	rcvObs = boost::posix_time::microsec_clock::universal_time();
	_blk->subscribeTo("behavior", 0);
	postX = yellowPostCenterX;
	postY = yellowPostCenterY;
	myPosX = 0.0;
	myPosY = 0.0;
	myPhi = 0.0;
	ballX = 0.0;
	ballY = 0.0;
	theta = 0.0;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	bhmsg = new BToHeadMessage();
	lastObs = new WorldInfo();
	headaction= BALLTRACK;
}

std::string TargetedApproach::GetName () {
	return "TargetedApproach";
}
	

void TargetedApproach::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("TargetedApproach",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void TargetedApproach::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}

float TargetedApproach::calculateAngle( float bPosX, float bPosY, float rPosX, float rPosY, float rPhi ){

	float theta = 0.0;
	float adjacent, hypotinuse;
	adjacent = ab.distance(postX, ballX, ballY, ballY);
	hypotinuse = ab.distance(postX, ballX, postY, ballY);
	
	theta = acos( adjacent/hypotinuse );
	
	if(myPhi>=0 && theta>=0)
		return fabs(myPhi-theta);
	if(myPhi>=0 && theta<0)
		return (-1)*fabs(myPhi-theta);
	if(myPhi<0 && theta<0)
		return (myPhi-theta);
	if(myPhi<0 && theta>=0)
		return (theta- myPhi);
}
