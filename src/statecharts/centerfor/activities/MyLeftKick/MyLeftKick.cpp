
#include "MyLeftKick.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<MyLeftKick>::Type temp("MyLeftKick");
}

int MyLeftKick::Execute() {
	_blk->process_messages();
	gsm = _blk->read_state<GameStateMessage>("GameStateMessage");
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING)
		return 0;
	//Logger::Instance().WriteMsg("MyLeftKick",  " Execute", Logger::Info);
	
	amot->set_command("LeftKick");
	_blk->publish_signal(*amot, "motion");
	
	return 0;
}

void MyLeftKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();

}

std::string MyLeftKick::GetName () {
	return "MyLeftKick";
}
	
