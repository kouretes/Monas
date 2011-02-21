
#include "MyLeftSideKick.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "tools/logger.h"
#include "tools/toString.h"

namespace {
    ActivityRegistrar<MyLeftSideKick>::Type temp("MyLeftSideKick");
}

int MyLeftSideKick::Execute() {
	gsm = _blk->read_state<GameStateMessage>("GameStateMessage");
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING)
		return 0;
	//Logger::Instance().WriteMsg("MyLeftSoftSIDEKick",  " ExecuteKICKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK", Logger::Info);
	amot->set_command("LeftSideKickFast.xar");
	_blk->publish_signal(*amot, "motion");
	return 0;
}

void MyLeftSideKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();

}

std::string MyLeftSideKick::GetName () {
	return "MyLeftSideKick";
}
