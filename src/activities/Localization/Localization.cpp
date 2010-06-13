#include "Localization.h"
#include "tools/logger.h"
#include "tools/toString.h"
#include "messages/RoboCupGameControlData.h"

namespace {
	ActivityRegistrar<Localization>::Type temp("Localization");
}

Localization::Localization() :
	Publisher("Localization") {
}

void Localization::UserInit() {

	_com->get_message_queue()->add_publisher(this);
	_com->get_message_queue()->add_subscriber(_blk);
	_com->get_message_queue()->subscribe("vision", _blk, 0);
	_com->get_message_queue()->subscribe("sensors", _blk, 0);
	_com->get_message_queue()->subscribe("Localization", _blk, 0);
	
	
	Logger::Instance().WriteMsg("Localization", "Localization Initialized", Logger::Info);

	hjsm = 0;
	bmsg = 0;
	gsm = 0;
}

int Localization::Execute() {

	read_messages();
	return 0;
}

void Localization::read_messages() {
	_blk->process_messages();
	gsm =  _blk->in_nb<GameStateMessage>("GameStateMessage", "RobotController");
	bmsg = _blk->in_nb<BallTrackMessage>("BallTrackMessage", "Vision");
	hjsm = _blk->in_nb<HeadJointSensorsMessage>("HeadJointSensorsMessage", "Sensors");

	Logger::Instance().WriteMsg("Localization", "read_messages ", Logger::ExtraExtraInfo);

}
