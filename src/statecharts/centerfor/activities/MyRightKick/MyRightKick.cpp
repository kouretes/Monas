#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "MyRightKick.h"
#include "messages/RoboCupGameControlData.h"
namespace {
    ActivityRegistrar<MyRightKick>::Type temp("MyRightKick");
}


int MyRightKick::Execute() {
	_blk->process_messages();
	gsm = _blk->read_state<GameStateMessage>("GameStateMessage");
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING)
		return 0;

	//Logger::Instance().WriteMsg("MyRightKick",  " Execute", Logger::Info);

	amot->set_command("RightKick");
	_blk->publish_signal(*amot, "motion");

	return 0;
}

void MyRightKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();

}

std::string MyRightKick::GetName () {
	return "MyRightKick";
}
	
