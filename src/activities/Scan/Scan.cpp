
#include "Scan.h"


namespace {
    ActivityRegistrar<Scan>::Type temp("Scan");
}

int Scan::Execute() {
	//Logger::Instance().WriteMsg("Scan",  " execute", Logger::Info);
	hbm = _blk->readState<HeadToBMessage> ("behavior");
	scnm = _blk->readSignal<ScanMessage> ("behavior");
	headaction = SCANFORBALL;
	
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
			//Logger::Instance().WriteMsg("Scan",  " BALLTRACK", Logger::Info);
		}
		else{		
			//if (lastTurn+boost::posix_time::seconds(5)< boost::posix_time::microsec_clock::universal_time() ){//&& lastTurn+boost::posix_time::seconds(8)>boost::posix_time::microsec_clock::universal_time()){
				
			//	littleWalk(0.0, 0.0, 45* TO_RAD);
			//	velocityWalk(0.0f, 0.0f, 0.0f , 1.0f);
				//if(times%2 ==0)
				//	side = (-1)*side;
			//	lastTurn = boost::posix_time::microsec_clock::universal_time();
				//times++;
				//if(times==1)
				//	times++;
		//	}
			//if(scnm!=0 && scnm->scancompleted()){
				//littleWalk(0.0f, 0.0f, 45*TO_RAD);
				//lastTurn = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(4);
			//}else{
				//if(lastTurn<= boost::posix_time::microsec_clock::universal_time())
					velocityWalk(0.0f, 0.0f, 0.0f, 1.0f);
			//}
			headaction = SCANFORBALL;
			//Logger::Instance().WriteMsg("Scan",  " SCANFORBALL", Logger::Info);
		}
	}
	bhmsg->set_headaction(headaction);
	_blk->publishSignal(*bhmsg, "behavior");
	
	tmsg = _blk->readState<TimeoutMsg>("behavior");
	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	if(now >boost::posix_time::from_iso_string(tmsg->wakeup())+ boost::posix_time::seconds(15)  && now < boost::posix_time::from_iso_string(tmsg->wakeup())+ boost::posix_time::seconds(60)){
		toPos = true;
		rpm->set_goalietopos(toPos);
		_blk->publishState(*rpm, "behavior");
	}
	else{
		if(toPos){
			toPos = false;
			rpm->set_goalietopos(toPos);
			_blk->publishState(*rpm, "behavior");
		}
	}
	
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

	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);
	wmot.add_parameter(0.0f);	
}

std::string Scan::GetName () {
	return "Scan";
}

void Scan::velocityWalk(double x, double y, double th, double f) {
	//Logger::Instance().WriteMsg("Aproachball",  " VelocityWalk", Logger::Info);
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
