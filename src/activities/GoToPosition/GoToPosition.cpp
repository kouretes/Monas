
#include "GoToPosition.h"

namespace {
    ActivityRegistrar<GoToPosition>::Type temp("GoToPosition");
}

int GoToPosition::Execute() {
	/*  */
	pm = _blk->readState<PositionMessage>("behavior");
	wimsg = _blk->readData<WorldInfo>("behavior");
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
	
	dist = ab.distance(posX, myPosX, posY, myPosY);
	relativePhi = myPhi-theta;
	relativeX = ab.rotation(posX, posY, relativePhi) - myPosX;
	relativeY = ab.rotation(posY, posX, relativePhi) - myPosY;
	
	if(dist<minDistGB){
		littleWalk( relativeX, relativeY, relativePhi);
	}
	else{
		if(fabs(posX-myPosX )>minDistGB && fabs(posY-myPosY )>minDistGB )
			littleWalk( relativeX/3, relativeY/3 , relativePhi );
		else if(fabs(posX-myPosX )>minDistGB && fabs(posY-myPosY )<=minDistGB )
			littleWalk(relativeX/3, relativeY , relativePhi );
		else if(fabs(posX-myPosX )<=minDistGB && fabs(posY-myPosY )>minDistGB )
			littleWalk(relativeX, relativeY/3 , relativePhi );
	}
		
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	return 0;
}

void GoToPosition::UserInit () {
	_blk->subscribeTo("behavior", 0);
	bhmsg = new BToHeadMessage();
	theta = 0.0;
	headaction = SCANFORPOST;
	posX = 0.70;
	posY = 0.70;
	dist = 0.0;
	relativeX =0.0;
	relativeY = 0.0; 
	relativePhi = 0.0;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	
}

std::string GoToPosition::GetName () {
	return "GoToPosition";
}

bool GoToPosition::robotInPosition(float x1, float x2, float y1, float y2, float th1, float th2){
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
