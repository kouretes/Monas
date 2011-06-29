
#include "MyLeftKick.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<MyLeftKick>::Type temp("MyLeftKick");
}

int MyLeftKick::Execute() {
	//_blk->process_messages();
	gsm = _blk->readState<GameStateMessage>("behavior");
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING){
	//	Logger::Instance().WriteMsg("MyLeftKick",  " No********************************", Logger::Info);
		return 0;
	}
	//Logger::Instance().WriteMsg("MyLeftKick",  " Execute********************************", Logger::Info);
	
	amot->set_command("LeftKick");
	_blk->publishSignal(*amot, "motion");
	
	rpm->set_goalietopos(true);
	_blk->publishSignal(*rpm, "behavior");
	return 0;
}

void MyLeftKick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
	rpm = new ReturnToPositionMessage();
}

std::string MyLeftKick::GetName () {
	return "MyLeftKick";
}
	
