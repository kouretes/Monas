
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
	gsm = _blk->read_state<GameStateMessage> ("GameStateMessage");
	prevaction = curraction;
	boost::posix_time::ptime timeout = boost::posix_time::microsec_clock::local_time()+boost::posix_time::millisec(2000);
	tmsg->set_wakeup(boost::posix_time::to_iso_string(timeout));
	_blk->publish_state(*tmsg, "behavior");
		if(gsm==0 ){
			bhmsg->set_headaction(DONOTHING);
			_blk->publish_signal(*bhmsg, "behavior");
			return 0;
		}	
		if(gsm->player_state()==PLAYER_PLAYING){
			std::cout <<"STATE NOPLAY PLAYER_PLAYING "<<std::endl;
			return 0;
		}
		if(gsm!=0){
			switch(gsm->player_state()){
					case PLAYER_PENALISED:
						std::cout <<"STATE NOPLAY playerpenalised "<<std::endl;
						//velocityWalk(0.0, 0.0, 0.0, 1);
						curraction = CALIBRATE;
						break;
					case PLAYER_SET:
						std::cout << "STATE NOPLAY playerset " <<std::endl;
						curraction = DONOTHING;
						kcm->set_kickoff(gsm->kickoff());
						_blk->publish_signal(*kcm, "behavior");
						break;
					case PLAYER_READY:
						std::cout << "STATE NOPLAY playerready " <<std::endl;
						curraction = CALIBRATE;
						break;
					case PLAYER_INITIAL:
						std::cout << "STATE NOPLAY playerinitial " <<std::endl;
						velocityWalk(0,0,0,1);
						curraction = DONOTHING;
						break;
			}
		}
		if (curraction== CALIBRATE && prevaction==CALIBRATE)
			bhmsg->set_headaction(DONOTHING);
		else
			bhmsg->set_headaction(curraction);

		_blk->publish_signal(*bhmsg, "behavior");
	return 0;
}

void NoPlay::UserInit () {
	_blk->subscribeTo("behavior", 0);
	//readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	tmsg = new TimeoutMsg();
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
	
	
bool NoPlay::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);
	std::cout<<"Read Configuration!!!"<<std::endl;
	int playernum = -1;
	//if (!config.QueryElement("player", playernum))
	//	Logger::Instance().WriteMsg("BodyBehavior", "Configuration file has no player, setting to default value: " + _toString(playernum), Logger::Error);

	//If color is changed default configuration color does need to be changed
	std::string color = "blue";
	teamColor = TEAM_BLUE;
	//if (!config.QueryElement("default_team_color", color))
	//	Logger::Instance().WriteMsg("BodyBehavior", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	//else
		//Logger::Instance().WriteMsg("BodyBehavior", "Undefined color in configuration, setting to default value: " + color, Logger::Error);

	return true;
}

void NoPlay::velocityWalk(double x, double y, double th, double f) {
	wmot->set_command("setWalkTargetVelocity");
	wmot->set_parameter(0, x);
	wmot->set_parameter(1, y);
	wmot->set_parameter(2, th);
	wmot->set_parameter(3, f);
	_blk->publish_signal(*wmot, "motion");
}

