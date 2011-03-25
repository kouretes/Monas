
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

	//Logger::Instance().WriteMsg("ProtectPost",  " Execute", Logger::Info);
	
	obsm = _blk->readSignal<ObservationMessage> ("vision");
	
	float bd = 0.0, bx = 0.0, by = 0.0, bb = 0.0;
	
	headaction = BALLTRACK;
	if (obsm.get()==0){
		//Logger::Instance().WriteMsg("ProtectPost",  " No OBS", Logger::Info);
		if (lastObsm==0)
			return 0;
		if (rcvObsm < boost::posix_time::microsec_clock::universal_time() ){
			littleWalk(0.005,0.005,0);
			headaction=SCANFORBALL;
			lastPostScan = boost::posix_time::microsec_clock::universal_time()+ boost::posix_time::seconds(1);
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
	
	if(bx<minDistGB && by<minDistGB){
		if(fabs(bx-dDistBallX)>dDistBallOffset && fabs(by-dDistBallY)>dDistBallOffset)
			littleWalk(bx-dDistBallX, by-dDistBallY, bb);
		else{
			if(bb>0)
				amot->set_command("left");
			else
				amot->set_command("right");
			_blk->publishSignal(*amot, "motion");
		}
	}	
	else
		blockBall();
		
	if(lastPostScan<boost::posix_time::microsec_clock::universal_time())
		headaction = SCANFORPOST;
	else if ( lastPostScan + boost::posix_time::seconds(3) < boost::posix_time::microsec_clock::universal_time()){
		lastPostScan = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::seconds(1);
		headaction = SCANFORPOST;
	}
	
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	
	return 0;
}

void ProtectPost::UserInit () {
	
	amot = new MotionActionMessage();
	rcvObsm = boost::posix_time::microsec_clock::universal_time();
	lastPostScan = boost::posix_time::microsec_clock::universal_time();
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("behavior", 0);
	myPosY = 0.0;
	myPosX = 2.6;
	myPhi = 0.0; 
	estBallX = 0.0;
	estBallY = 0.0;
	bhmsg = new BToHeadMessage();
	lastObsm = new ObservationMessage();
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
	wimsg = _blk->readData<WorldInfo>("behavior");
	
	if(wimsg.get()!=0){
		myPosX = wimsg->myposition().x();
		myPosY = wimsg->myposition().y();
		myPhi = wimsg->myposition().phi();
		if(wimsg->balls_size()>0){
			estBallX = ab.rotation(wimsg->balls(0).relativex(), wimsg->balls(0).relativey(), myPhi) + myPosX;
			estBallY = ab.rotation( wimsg->balls(0).relativey(), wimsg->balls(0).relativex(), myPhi) + myPosY;
		}
	}
	
	float th1=0.0, th2=0.0, x=0.0, y=0.0;
	
	float dist;
	if(wimsg->balls_size()>0){
		th1 = atan(( limitX - estBallX)/estBallY);
		
		th2 = atan (( limitX - estBallX)/(estBallY-lPost));
		
		dist = ab.distance(limitX,estBallX,  estBallY,0) - robotWidth/tan(th2-th1);
		
		x = cos(th1)*dist;
		y = limitX - sin(th1)*dist;
		
		littleWalk((myPosX-x), (myPosY-y),myPhi - th1);
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
