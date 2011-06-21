
#include "Kick.h"

namespace {
    ActivityRegistrar<Kick>::Type temp("Kick");
}

int Kick::Execute() {
	gsm = _blk->readState<GameStateMessage>("behavior");
	wimsg  = _blk->readData<WorldInfo>("behavior");
	
	if(gsm!=0 && gsm->game_state()!=PLAYER_PLAYING){
	//	Logger::Instance().WriteMsg("MyLeftKick",  " No********************************", Logger::Info);
		return 0;
	}
	//Logger::Instance().WriteMsg("MyLeftKick",  " Execute********************************", Logger::Info);
	if (wimsg==0 || wimsg->balls_size()==0){
			//Logger::Instance().WriteMsg("TrCond_0_3_2_3_4_6_2TOMyLeftKick", "FALSE NO OBSM", Logger::Info);
			return 0;
	}else if ( wimsg->balls(0).relativey() > -0.01 ){
		amot->set_command("LeftKick");
		_blk->publishSignal(*amot, "motion");
		//Logger::Instance().WriteMsg("Kick", "LeftKick", Logger::Info);
		return 0;
	}else{
		amot->set_command("RightKick");
		_blk->publishSignal(*amot, "motion");
	//	Logger::Instance().WriteMsg("Kick", "RightKick", Logger::Info);
		return 0;
	}
	return 0;
}

void Kick::UserInit () {
	_blk->subscribeTo("behavior",0);
	amot = new MotionActionMessage();
}

std::string Kick::GetName () {
	return "Kick";
}
	
