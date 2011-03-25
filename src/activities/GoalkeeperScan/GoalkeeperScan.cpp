
#include "GoalkeeperScan.h"

namespace {
    ActivityRegistrar<GoalkeeperScan>::Type temp("GoalkeeperScan");
}

int GoalkeeperScan::Execute() {
	np.readRobotConfiguration(ArchConfig::Instance().GetConfigPrefix() + "robotConfig", false);
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	wimsg = _blk->readData<WorldInfo> ("behavior");
	pm = _blk->readState<PositionMessage>("behavior");

	headaction = SCANFORBALL;
	float dist;
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
	relativeX = ab.rotation(posX, posY, relativePhi);
	relativeY = ab.rotation(posY, posX, relativePhi);
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			//Logger::Instance().WriteMsg("Scan",  " BALLTRACK", Logger::Info);
		}
		else{		
			
			if(pm->posx() - locDeviation > wimsg->myposition().x() || wimsg->myposition().x() > pm->posx() + locDeviation || pm->posy() - locDeviation > wimsg->myposition().y() || wimsg->myposition().y() > pm->posy() + locDeviation){
				littleWalk(relativeX,relativeY, relativePhi);
			}
			if (lastTurn< boost::posix_time::microsec_clock::universal_time()){
				littleWalk( 0.0, 0.0, 45 * TO_RAD);
				lastTurn = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(5);
			}else{
				velocityWalk( 0.0, 0.0, 0.0, 1);		
			}
			//headaction = SCANFORBALL;
			//Logger::Instance().WriteMsg("Scan",  " SCANFORBALL", Logger::Info);
		}
	}
	
	if(headaction!=BALLTRACK){
		if(scanPost){
			headaction = SCANFORPOST;
			if(changeScan+boost::posix_time::seconds(1)<boost::posix_time::microsec_clock::universal_time()){
				changeScan = boost::posix_time::microsec_clock::universal_time();
				scanPost = false;
			}
		}
		else{
			if(changeScan+boost::posix_time::seconds(3)<boost::posix_time::microsec_clock::universal_time()){
				changeScan = boost::posix_time::microsec_clock::universal_time();
				scanPost = true;
			}
			
		}
	}
		
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	return 0;
}

void GoalkeeperScan::UserInit () {
	_blk->subscribeTo("behavior", 0);
	headaction = SCANFORBALL;
	bhmsg = new BToHeadMessage();
	lastTurn = boost::posix_time::microsec_clock::universal_time();
	changeScan  = boost::posix_time::microsec_clock::universal_time();
	scanPost = false;
	//wmot = new MotionWalkMessage();
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);	
}

std::string GoalkeeperScan::GetName () {
	return "GoalkeeperScan";
}
void GoalkeeperScan::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Approachball",  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void GoalkeeperScan::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}
