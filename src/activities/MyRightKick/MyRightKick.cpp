#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "MyRightKick.h"
#include "messages/RoboCupGameControlData.h"
namespace {
    ActivityRegistrar<MyRightKick>::Type temp("MyRightKick");
}


int MyRightKick::Execute() {
	//_blk->process_messages();
	gsm = _blk->readState<GameStateMessage>("behavior");
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING){
		//Logger::Instance().WriteMsg("MyRightKick",  " No********************************", Logger::Info);
		return 0;
	}

	//Logger::Instance().WriteMsg("MyRightKick",  " Execute***************************", Logger::Info);

	amot->set_command("RightKick");
	_blk->publishSignal(*amot, "motion");

	rpm->set_goalietopos(true);
	_blk->publishSignal(*rpm, "behavior");

	return 0;
}

void MyRightKick::UserInit () {
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	amot = new MotionActionMessage();
	rpm = new ReturnToPositionMessage();
}

std::string MyRightKick::GetName () {
	return "MyRightKick";
}

