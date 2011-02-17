
#include "MyLeftKick.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<MyLeftKick>::Type temp("MyLeftKick");
}

int MyLeftKick::Execute() {
	//gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	//if (gsm==0)
		//return 0;
	//if(gsm!=0 && gsm->player_state()!=PLAYER_PLAYING){
		//std::cout <<"STATE MYLEFTKICK NOT PLAYER_PLAYING "<<std::endl;
		//return 0;
	//}
	std::cout << "STATE MYLEFTKICK" <<std::endl;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::universal_time()+boost::posix_time::millisec(2000);
	tmsg->set_wakeup(boost::posix_time::to_iso_string(timeout));
	_blk->publish_state(*tmsg, "behavior");
	amot->set_command("LeftKick");
	_blk->publish_signal(*amot, "motion");
	return 0;
}

void MyLeftKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	tmsg = new TimeoutMsg();
}

std::string MyLeftKick::GetName () {
	return "MyLeftKick";
}
	
