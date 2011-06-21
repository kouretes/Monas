
#include "Scan.h"


namespace {
    ActivityRegistrar<Scan>::Type temp("Scan");
}

int Scan::Execute() {
	Logger::Instance().WriteMsg("Scan",  " execute", Logger::Info);
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	scnm = _blk->readSignal<ScanMessage> ("behavior");
	wimsg = _blk->readData<WorldInfo>("behavior");
	pm = _blk->readState<PositionMessage>("behavior");
	
	//if(headaction== SCANFORBALL){
		//lastScanForBall =  boost::posix_time::microsec_clock::universal_time();
	//}
	headaction = SCANFORBALL;
	velocityWalk(0.0f,0.0f, 0.0f, 1.0f);
//	rtm = _blk->readSignal<RestartTurnMessage> ("behavior");
//	if(rtm.get()!=0 && rtm->restartnow()==true)
//		times=0;
	LedChangeMessage leds;
	LedValues* l = leds.add_leds();
	l->set_chain("r_ear");
	l->set_color( "off");
	_blk->publishSignal(leds, "leds");	
	if( hbm.get()!=0 ){
		if( hbm->ballfound()>0){
			headaction = BALLTRACK;
			//Logger::Instance().WriteMsg(GetName (),  " BALLTRACK", Logger::Info);
		}
		else{		

			//if(scnm!=0 && scnm->scancompleted()){
				//littleWalk(0.0f, 0.0f, 45*TO_RAD);
				//lastTurn = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(4);
			//}else{
				//if(lastTurn<= )
					velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
			//}
			headaction = SCANFORBALL;
			//Logger::Instance().WriteMsg(GetName (),  " SCANFORBALL", Logger::Info);
		}
	}

	if(wimsg!=0 && wimsg->myposition().confidence() <goodConfidence && pm!=0 && !g.robotInPosition(wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(), pm->posy(), wimsg->myposition().phi(), pm->theta() ) && robotInGoalPostArea(wimsg->myposition().x(), pm->posx(), wimsg->myposition().y(), pm->posy(), wimsg->myposition().phi(), pm->theta() )){
		//if(lastScanForBall + boost::posix_time::seconds(3)<boost::posix_time::microsec_clock::universal_time())
		headaction = SCANFORPOST;
		float vely = pm->posy() - wimsg->myposition().y();
		vely = vely>1 ? 1:vely;
		vely = vely<-1 ? -1:vely;
		velocityWalk(0.0f, vely, 0.0, 0.8);
	}
		
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	
	return 0;
}

void Scan::UserInit () {
	_blk->subscribeTo("behavior", 0);
	_blk->subscribeTo("vision", 0);
	rpm  = new ReturnToPositionMessage();
	toPos = false;
	headaction = SCANFORBALL;
	//side = 1;
	//times = 0;
	bhmsg = new BToHeadMessage();
	lastTurn = boost::posix_time::microsec_clock::universal_time();
	lastScanForBall = boost::posix_time::microsec_clock::universal_time();

	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);	
}

std::string Scan::GetName () {
	return "Scan";
}

void Scan::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg(GetName (),  " VelocityWalk", Logger::Info);
	wmot.set_command("setWalkTargetVelocity");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	wmot.set_parameter(3, f);
	_blk->publishSignal(wmot, "motion");
}

void Scan::littleWalk(double x, double y, double th) {
	//cout << x << " y " << y << " th " << th<<endl;
	wmot.set_command("walkTo");
	wmot.set_parameter(0, x);
	wmot.set_parameter(1, y);
	wmot.set_parameter(2, th);
	_blk->publishSignal(wmot, "motion");
}


bool Scan::robotInGoalPostArea(float rx, float x2, float ry, float y2, float rth, float th2){
	
	
	Logger::Instance().WriteMsg("robotIposition",  " entered", Logger::Info);
	if( x2 - 0.3 > rx || rx > x2 + 0.3 )
		return false;	
	if( y2 - 1.1 > ry || ry > y2 + 1.1  )
		return false;
	if( th2 - th2*0.2 > rth || rth > th2 + th2*0.2  )
		return false;
	return true;
	
	
}
