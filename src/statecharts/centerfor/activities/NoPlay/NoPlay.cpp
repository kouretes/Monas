
#include "NoPlay.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<NoPlay>::Type temp("NoPlay");
}

int NoPlay::Execute() {
	
	//Logger::Instance().WriteMsg("NoPlay",  " Execute", Logger::Info);

	gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");

		if(gsm.get()==0 ){
		//	Logger::Instance().WriteMsg("NoPlay",  " NO GSM", Logger::Info);
			bhmsg->set_headaction(DONOTHING);
		}else if(gsm->player_state()==PLAYER_PLAYING){
			//Logger::Instance().WriteMsg("NoPlay",  " PLAYER_PLAYING", Logger::Info);	
			return 0;
		}else{
			switch(gsm->player_state()){
				case PLAYER_PENALISED:
				//	Logger::Instance().WriteMsg("NoPlay",  " playerpenalised", Logger::Info);
					//if(prevaction!=CALIBRATE)
						velocityWalk(0.0, 0.0, 0.0, 1);
					curraction = CALIBRATE;
				break;
				case PLAYER_SET:
				//	Logger::Instance().WriteMsg("NoPlay",  " playerset", Logger::Info);
					curraction = DONOTHING;
					kcm->set_kickoff(gsm->kickoff());
					_blk->publish_state(*kcm, "behavior");
				break;
				case PLAYER_READY:
				//	Logger::Instance().WriteMsg("NoPlay",  " playerready", Logger::Info);
					curraction = CALIBRATE;
				break;
				case PLAYER_INITIAL:
				//	Logger::Instance().WriteMsg("NoPlay",  " playerinitial", Logger::Info);
					velocityWalk(0,0,0,1);
					curraction = DONOTHING;
				break;
			}
		
						
	}
	prevaction = curraction;
	bhmsg->set_headaction(curraction);	
	_blk->publish_state(*bhmsg, "behavior");
		
			return 0;

}

void NoPlay::UserInit () {
	_blk->subscribeTo("behavior", 0);
	curraction = 0;
	prevaction = 0;
	bhmsg = new BToHeadMessage();
	kcm = new KickOffMessage();
	wmot = new MotionWalkMessage();
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	wmot->add_parameter(0.0f);
	
}

std::string NoPlay::GetName () {
	return "NoPlay";
}

void NoPlay::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

