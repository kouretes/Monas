
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
	//_blk->process_messages();
	//_blk->updateSubscription("behavior", msgentry::SUBSCRIBE_ON_TOPIC);
	//readConfiguration(ArchConfig::Instance().GetConfigPrefix() + "/team_config.xml");
	//Logger::Instance().WriteMsg("Init",  " Execute "+ _toString(playernum), Logger::Info);
//
//_blk->publish_all();
	return 0;
}

void Init::UserInit () {
//	pnm = new PlayerNumberMessage();


}

std::string Init::GetName () {
	return "Init";
}
