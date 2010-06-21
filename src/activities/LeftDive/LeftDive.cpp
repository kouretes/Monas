
#include "LeftDive.h"

namespace {
    ActivityRegistrar<LeftDive>::Type temp("LeftDive");
}


LeftDive::LeftDive() : Publisher("LeftDive")
{
  ;
}


int LeftDive::Execute() {
        MotionActionMessage msg;
        msg.set_command("leftDive");
        publish(&msg,"motion");
	return 0;
}

void LeftDive::UserInit () {
	_com->get_message_queue()->add_publisher(this);
}

std::string LeftDive::GetName () {
	return "LeftDive";
}
