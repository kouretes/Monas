
#include "GoToPosition.h"
#include "architecture/archConfig.h"
#include "tools/XML.h"
#include "tools/XMLConfig.h"
namespace {
    ActivityRegistrar<GoToPosition>::Type temp("GoToPosition");
}

int GoToPosition::Execute() {
	/*  */
//	Logger::Instance().WriteMsg(GetName(),  " Execute ", Logger::Info);
	pm = _blk->readState<PositionMessage>("behavior");
	wimsg = _blk->readData<WorldInfo>("behavior");
	obsm = _blk->readSignal<ObservationMessage>("vision");
	gsm =  _blk->readState<GameStateMessage>("behavior");
	headaction = SCANFORPOST;
	if(pm.get()!=0){		///get my target
		posX = pm->posx();
		posY = pm->posy();
		theta = pm->theta();
	}

	if(wimsg.get()!=0){		///get my position
		myPosX = wimsg->myposition().x();
		myPosY = wimsg->myposition().y();
		myPhi = wimsg->myposition().phi();
		confidence =  wimsg->myposition().confidence();
	}

	if(obsm&&obsm->regular_objects_size()>0)
		lastObsm = microsec_clock::universal_time();

	//Logger::Instance().WriteMsg(GetName(),  "Init X "+ _toString(posX) +" InitY " + _toString(posY) + " InitPhi " + _toString(theta), Logger::Info);
	//Logger::Instance().WriteMsg(GetName(),  "Pos X "+ _toString(myPosX) +" Pos Y " + _toString(myPosY) + " Phi " + _toString(myPhi) + " Confidence " + _toString(confidence), Logger::Info);
	robotInPos = robotInPosition(posX, myPosX, posY, myPosY, theta, myPhi);
	
	ripm.set_inposition(robotInPos);
	_blk->publishSignal(ripm, "behavior");
	
	if(robotInPos && lastMove <= microsec_clock::universal_time() ){
		velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
		lastMove = microsec_clock::universal_time() + milliseconds(500);
		Logger::Instance().WriteMsg(GetName(), "Robot Is In Position", Logger::Info);
		_blk->publish_all();
		return 0;
	}

	if(confidence<badConfidence && lastMove <= microsec_clock::universal_time()){

		if(lastObsm + seconds(10) <microsec_clock::universal_time() ){
			velocityWalk(0.5, 0.5, 0.0, 0.7);
			lastMove = microsec_clock::universal_time()+ milliseconds(500);
		}
		//else{
			//velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
			//lastMove = microsec_clock::universal_time() + milliseconds(500);
		//}
		_blk->publish_all();
		return 0;
	}

	dist = distance(posX, myPosX, posY, myPosY);

	float angleToTarget = anglediff2(atan2(posY -myPosY, posX - myPosX), myPhi);
	relativePhi = anglediff2(theta,myPhi);
	float rot = 0.0, f=1.0;
	float velx, vely;
	vely = sin(angleToTarget);
	velx = cos(angleToTarget);


	if(dist <0.3){
		velx/=2.0;
		vely/=2.0;
		rot = relativePhi*0.5;
		f = dist*2;
	}else if(dist>1 && gsm!=0 && gsm->player_state()==PLAYER_PLAYING){
		velx/=4.0;
		vely/=4.0;
		rot = relativePhi*0.2;
		f = 0.6;
		if(confidence>goodConfidence){
			f=1;
			velx*=2;
			vely*=2;
		}
	}else if(dist>1 &&(gsm==0 ||(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING))){
		velx/=4.0;
		vely/=4.0;
		rot = angleToTarget*0.4;
		f = 0.6;
		if(confidence>goodConfidence){
			f=1;
			velx*=2;
			vely*=2;
		}
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

	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	_blk->publish_all();
	return 0;
}

void GoToPosition::UserInit () {
	robotInPos = false;
	confidence = 0;
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	_blk->updateSubscription("vision", msgentry::SUBSCRIBE_ON_TOPIC);
	int playernum, teamColor;
	lastMove = microsec_clock::universal_time();
	lastObsm = microsec_clock::universal_time();
	bhmsg = new BToHeadMessage();
	theta = 0.0;
	headaction = SCANFORPOST;
	posX = -2.50;
	posY = 0.0;
	dist = 0.0;
	relativePhi = 0.0;

	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);

	//Logger::Instance().WriteMsg(GetName(),  " UserInit ", Logger::Info);
}

std::string GoToPosition::GetName () {
	return "GoToPosition";
}

bool GoToPosition::robotInPosition(float currentX, float targetX, float currentY, float targetY, float currentTheta, float targetTheta){
	//Logger::Instance().WriteMsg("robotIposition",  " entered", Logger::Info);
	if( targetX - locDeviation > currentX || currentX > targetX + locDeviation )
		return false;
	if( targetY - locDeviation > currentY || currentY > targetY + locDeviation  )
		return false;
	if( anglediff2(targetTheta , currentTheta)> M_PI/8  )
		return false;
	return true;
}

void GoToPosition::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg(GetName(),  " VelocityWalk", Logger::Info);
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
	//Logger::Instance().WriteMsg("Rotation",  " Entered", Logger::Info);
	return a*cos(theta) + b*sin(theta);


}

float GoToPosition::distance(float x1, float x2, float y1, float y2){
	//Logger::Instance().WriteMsg("Distance",  " Entered", Logger::Info);
	float dis;
	dis = sqrt((pow((x2-x1), 2)+ pow((y2-y1), 2)));

	return dis;
}
