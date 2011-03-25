
#include "KickOff.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<KickOff>::Type temp("KickOff");
}

int KickOff::Execute() {
	//sleep(1);	
//gsm = _blk->readState<GameStateMessage> ("behavior");
	//if (gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE KICKOFF NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	//_blk->process_messages();
	//std::cout << "STATE KICKOFF" <<std::endl;
	//boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time();
	obsm = _blk->readSignal<ObservationMessage> ("ObservationMessage");
	Logger::Instance().WriteMsg("KickOff",  " Execute", Logger::Info);
	if(obsm.get()!=0){
		//std::cout << "STATE KICKOFF Kick" <<std::endl;
		Logger::Instance().WriteMsg("KickOff",  " Kicking", Logger::Info);
		if (obsm->ball().dist() * sin(obsm->ball().bearing()) > 0.0) 
			amot->set_command("SoftLeftSideKick");	
		 else 
			amot->set_command("SoftRightSideKick");	
	}
	_blk->publishSignal(*amot, "motion");
	kcm->set_kickoff(false);
	_blk->publishState(*kcm, "behavior");
	return 0;
}

void KickOff::UserInit () {
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	tmsg = new TimeoutMsg();
	kcm = new KickOffMessage();
}

std::string KickOff::GetName () {
	return "KickOff";
}
	
