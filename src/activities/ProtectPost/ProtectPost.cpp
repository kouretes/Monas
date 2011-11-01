
#include "ProtectPost.h"
#include "messages/RoboCupGameControlData.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>


namespace {
    ActivityRegistrar<ProtectPost>::Type temp("ProtectPost");
}

int ProtectPost::Execute() {

	Logger::Instance().WriteMsg("ProtectPost",  " Execute", Logger::Info);

	wimsg = _blk->readData<WorldInfo>("behavior");

	blockBall();

	headaction = BALLTRACK;
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");

	rpm->set_goalietopos(true);
	_blk->publishSignal(*rpm, "behavior");
	return 0;
}

void ProtectPost::UserInit () {
	rpm = new ReturnToPositionMessage();
	amot = new MotionActionMessage();
	rcvObs = microsec_clock::universal_time();
	lastPostScan =microsec_clock::universal_time();
	lastMove = microsec_clock::universal_time();
	//_blk->subscribeTo("vision", 0);
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	myPosY = 0.0;
	myPosX = 2.6;
	myPhi = 0.0;
	estBallX = 0.0;
	estBallY = 0.0;
	bhmsg = new BToHeadMessage();
	lastObs = new WorldInfo();
	headaction= BALLTRACK;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
}

std::string ProtectPost::GetName () {
	return "ProtectPost";
}

void ProtectPost::blockBall(){


	if(wimsg.get()!=0){
		myPosX = wimsg->myposition().x();
		myPosY = wimsg->myposition().y();
		myPhi = wimsg->myposition().phi();
		if(wimsg->balls_size()>0){
			//estBallX = ab.rotation(wimsg->balls(0).relativex(), wimsg->balls(0).relativey(), myPhi) + myPosX;
			estBallX = rotation(wimsg->balls(0).relativex(), wimsg->balls(0).relativey(), myPhi) + myPosX;
			//estBallY = ab.rotation( wimsg->balls(0).relativey(), wimsg->balls(0).relativex(), myPhi) + myPosY;
			estBallY = rotation( wimsg->balls(0).relativey(), wimsg->balls(0).relativex(), myPhi) + myPosY;
		}
	}
	int sidex=1, sidey =1;
	if(myPosX<0)
		sidex=-1;
	if(estBallY<0)
		sidey =-1;
	float th1=0.0, th2=0.0,th3=0.0, x=0.0, y=0.0;

	float dist1, dist2, distFromPostCenter;
	if(wimsg->balls_size()>0){
		th1 = atan(( limitX - sidex*estBallX)/estBallY); // ball to postcenter angle
		th2 = atan (( limitX - sidex*estBallX)/(estBallY-sidey*lPost)); //ball to leftpost timber angle
		th3 = atan(( limitX - sidex*estBallX)/(estBallY-sidey*rPost)); //ball to rightpost timber angle
		distFromPostCenter = distance(limitX,estBallX,  estBallY,0);
		//dist = ab.distance(limitX,estBallX,  estBallY,0) - robotWidth/tan(th2-th1);
		dist1 = distFromPostCenter - robotWidth/tan(th2-th1); //cover left side
		dist2 = distFromPostCenter - robotWidth/tan(th1-th3);	//cover right side

		if(dist1<dist2)
			dist1 = dist2;
		if(dist1>1.0)
			dist1 = 1.0;

		targety = cos(th1)*dist1;
		targetx = limitX - sin(th1)*dist1;

		GoToPosition(targetx, targety, th1);
		//littleWalk((myPosX-x), (myPosY-y),myPhi - th1);

	}
}

void ProtectPost::GoToPosition(float x, float y, float theta){

	float dist = distance(x, myPosX, y, myPosY);
	float angleToTarget = KLocalization::anglediff2(atan2(y -myPosY, x - myPosX), myPhi);
	float relativePhi = KLocalization::anglediff2(theta,myPhi);
	float rot = 0.0, f=1.0;
	float velx, vely;
	vely = sin(angleToTarget);
	velx = cos(angleToTarget);

	if(dist <0.3){
		velx/=2.0;
		vely/=2.0;
		rot = relativePhi*0.5;
		f = dist*2;
	}else if(dist>1){
		velx/=4.0;
		vely/=4.0;
		rot = angleToTarget*0.4;
		f = 0.6;
	}else{
		rot = angleToTarget*0.1;
		f = 1;
	}
	rot = rot>1.0 ? 1.0:rot;
	rot = rot<-1.0 ? -1.0:rot;
	velx = velx>1.0 ? 1.0: velx;
	velx = velx<-1.0 ? -1.0 : velx;
	vely = vely>1.0 ? 1.0: vely;
	vely = vely<-1.0 ? -1.0 : vely;
	//Logger::Instance().WriteMsg(GetName(),  " if", Logger::Info);
	if(lastMove <= microsec_clock::universal_time()){
		Logger::Instance().WriteMsg(GetName(),  " walk", Logger::Info);

		velocityWalk(velx, vely, rot, f);
		lastMove = microsec_clock::universal_time() + milliseconds(500);
	}


}
void ProtectPost::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Aproachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void ProtectPost::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}


/////////////////////////////////////////////////////////////////////
float ProtectPost::rotation(float a, float b, float theta){

	return a*cos(theta) + b*sin(theta);


}

float ProtectPost::distance(float x1, float x2, float y1, float y2){

	float dis;
	dis = sqrt(pow(x2-x1, 2)+ pow(y2-y1, 2));

	return dis;
}
