
#include "Init.h"
#include "architecture/archConfig.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"
#include <boost/date_time/posix_time/ptime.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
namespace {
    ActivityRegistrar<Init>::Type temp("Init");
}

int Init::Execute() {
	_blk->process_messages();
	_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	Logger::Instance().WriteMsg("Init",  " Execute "+ _toString(playernum), Logger::Info);


	return 0;
}

void Init::UserInit () {
	pnm = new PlayerNumberMessage();


}

std::string Init::GetName () {
	return "Init";
}


bool Init::readConfiguration(const std::string& file_name) {
	XMLConfig config(file_name);
	//int playernum = -1;
	if (!config.QueryElement("player", playernum))
		Logger::Instance().WriteMsg("Init", "Configuration file has no player, setting to default value: " + _toString(playernum), Logger::Error);

	//If color is changed default configuration color does need to be changed
	std::string color = "blue";
	teamColor = TEAM_BLUE;
	if (!config.QueryElement("default_team_color", color))
		Logger::Instance().WriteMsg("Init", "Configuration file has no team_color, setting to default value: " + color, Logger::Error);
	if (color == "blue")
		teamColor = TEAM_BLUE;
	else if (color == "red")
		teamColor = TEAM_RED;
	else
		Logger::Instance().WriteMsg("Init", "Undefined color in configuration, setting to default value: " + color, Logger::Error);
	pnm->set_team_side(teamColor);
	pnm->set_player_number(playernum);
	_blk->publishState(*pnm, "behavior");
	return true;
}
