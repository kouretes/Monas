
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
	gsm = _blk->readState<GameStateMessage>("behavior");
	if(gsm.get()==0 || (gsm.get()!=0 && gsm->game_state()!=PLAYER_PLAYING)){
		Logger::Instance().WriteMsg("MyLeftSoftSIDEKick",  " PLAYER NOT PLAYING", Logger::Info);
		return 0;
	}
	if(gsm->game_state()==PLAYER_PLAYING) {
		Logger::Instance().WriteMsg("MyLeftSoftSIDEKick",  " ExecuteKICKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK", Logger::Info);
		amot->set_command("LeftSideKickFast.xar");
		_blk->publishSignal(*amot, "motion");
		return 0;
	}
	return 0;
}

void MyLeftSideKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();

}

std::string MyLeftSideKick::GetName () {
	return "MyLeftSideKick";
}
