
#include "KickOff.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<KickOff>::Type temp("KickOff");
}

int KickOff::Execute() {
	 _blk->process_messages();
//gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	//if (gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE KICKOFF NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	std::cout << "STATE KICKOFF" <<std::endl;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time();
	obsm = _blk->read_signal<ObservationMessage> ("ObservationMessage");
	if(obsm!=0){
		std::cout << "STATE KICKOFF Kick" <<std::endl;
		if (obsm->ball().dist() * sin(obsm->ball().bearing()) > 0.0) 
			amot->set_command("SoftLeftSideKick");	
		 else 
			amot->set_command("SoftRightSideKick");	
	}
	_blk->publish_signal(*amot, "motion");
	return 0;
}

void KickOff::UserInit () {
	_blk->subscribeTo("vision", 0);
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	tmsg = new TimeoutMsg();
}

std::string KickOff::GetName () {
	return "KickOff";
}
	
