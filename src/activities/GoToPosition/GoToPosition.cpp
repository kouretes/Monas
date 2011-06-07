
#include "GoToPosition.h"
#include "architecture/archConfig.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
namespace {
    ActivityRegistrar<GoToPosition>::Type temp("GoToPosition");
}

int GoToPosition::Execute() {
	/*  */
//	Logger::Instance().WriteMsg("GoToPosition",  " Execute ", Logger::Info);
	pm = _blk->readState<PositionMessage>("behavior");
	wimsg = _blk->readData<WorldInfo>("behavior");
	obsm = _blk->readSignal<ObservationMessage>("vision");
	
	headaction = SCANFORPOST;
	if(pm.get()!=0){
		posX = pm->posx();
		posY = pm->posy();
		theta = pm->theta();
	}
	if(wimsg.get()!=0){
		myPosX = wimsg->myposition().x();
		myPosY = wimsg->myposition().y();
		myPhi = wimsg->myposition().phi();
	}
	Logger::Instance().WriteMsg("GoToPosition",  "Init X "+ _toString(posX) +" InitY " + _toString(posY) + " InitPhi " + _toString(theta), Logger::Info);
	Logger::Instance().WriteMsg("GoToPosition",  "Pos X "+ _toString(myPosX) +" Pos Y " + _toString(myPosY) + " Phi " + _toString(myPhi), Logger::Info);
	if(robotInPosition(posX, myPosX, posY, myPosY, theta, myPhi)){
		velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
		return 0;
	}
		
	if(obsm&&obsm->regular_objects_size() > 0)
		lastObsm = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(2);
		
	dist = distance(posX, myPosX, posY, myPosY);

	relativeX = rotation(posX, -posY, myPhi) - myPosX;

	relativeY = rotation(posY, posX, myPhi) - myPosY;

	KLocalization h;
	float angle = h.anglediff2(atan2(posY -myPosY, posX - myPosX), myPhi);
	relativePhi = h.anglediff2(theta,myPhi);
	
	float velx, vely;
	vely = 0.8*sin(angle);
	velx = 0.8*cos(angle);
		
	velx = velx>1 ? 1:velx;
	velx = velx<-1 ? -1:velx;
	vely = vely>1 ? 1:vely;
	vely = vely<-1 ? -1:vely;

	Logger::Instance().WriteMsg("GoToPosition",  " if", Logger::Info);
	if(lastMove <= boost::posix_time::microsec_clock::universal_time()){
		Logger::Instance().WriteMsg("GoToPosition",  " walk", Logger::Info);
		velocityWalk(velx, vely, 0.1*angle, 1.0);
		lastMove = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(500);
	}
	
	if(lastObsm<= boost::posix_time::microsec_clock::universal_time()){
		Logger::Instance().WriteMsg("GoToPosition",  " Scan for ball", Logger::Info);
		bhmsg->set_headaction(SCANFORBALL);
		_blk->publishSignal(*bhmsg, "behavior");
		return 0;
		}
		
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	return 0;
}

void GoToPosition::UserInit () {
	curr = prev = 0;
	_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("vision", 0);
	int playernum, teamColor;
	lastMove = boost::posix_time::microsec_clock::universal_time();
	lastObsm = boost::posix_time::microsec_clock::universal_time();
	bhmsg = new BToHeadMessage();
	theta = 0.0;
	headaction = SCANFORPOST;
	pmsg = new PositionMessage();
	posX = -2.50;
	posY = 0.0;
	dist = 0.0;
	relativeX =0.0;
	relativeY = 0.0; 
	relativePhi = 0.0;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);

	Logger::Instance().WriteMsg("GoToPosition",  " UserInit ", Logger::Info);
}

std::string GoToPosition::GetName () {
	return "GoToPosition";
}

bool GoToPosition::robotInPosition(float x1, float x2, float y1, float y2, float th1, float th2){
	Logger::Instance().WriteMsg("robotIposition",  " entered", Logger::Info);
	if( x1 - locDeviation > x2 || x2 > x1 + locDeviation )
		return false;	
	if( y1 - locDeviation > y2 || y2 > y1 + locDeviation )
		return false;
	if( th1 - 0.1*th1 > th2 || th2 > th1 + 0.1*th1  )
		return false;
	return true;
}

void GoToPosition::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void GoToPosition::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}
///////////////////////////////////////
float GoToPosition::rotation(float a, float b, float theta){
	Logger::Instance().WriteMsg("Rotation",  " Entered", Logger::Info);
	return a*cos(theta) + b*sin(theta);
	
	
}

float GoToPosition::distance(float x1, float x2, float y1, float y2){
	Logger::Instance().WriteMsg("Distance",  " Entered", Logger::Info);
	float dis;
	dis = sqrt(pow(x2-x1, 2)+ pow(y2-y1, 2));
	
	return dis;
}
