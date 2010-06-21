
#include "RightDive.h"

namespace {
    ActivityRegistrar<RightDive>::Type temp("RightDive");
}

RightDive::RightDive() : Publisher("RightDive")
{
  ;
}


int RightDive::Execute() {
  MotionActionMessage msg;
  msg.set_command("rightDive");
  publish(&msg,"motion");
  return 0;
}

void RightDive::UserInit () {
  _com->get_message_queue()->add_publisher(this);
}

std::string RightDive::GetName () {
	return "RightDive";
}
