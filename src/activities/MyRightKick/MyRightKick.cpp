#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "MyRightKick.h"
#include "messages/RoboCupGameControlData.h"
namespace {
    ActivityRegistrar<MyRightKick>::Type temp("MyRightKick");
}


int MyRightKick::Execute() {
	//gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	//if (gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE MYRIGHTKICK NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	std::cout << "STATE MYRIGHTKICK" <<std::endl;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time();
	tmsg->set_wakeup(boost::posix_time::to_iso_string(timeout));
	_blk->publish_state(*tmsg, "behavior");
	amot->set_command("RightKick");
	_blk->publish_signal(*amot, "motion");
	return 0;
}

void MyRightKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	tmsg = new TimeoutMsg();
}

std::string MyRightKick::GetName () {
	return "MyRightKick";
}
	
