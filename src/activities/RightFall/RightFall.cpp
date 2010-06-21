
#include "RightFall.h"

namespace {
    ActivityRegistrar<RightFall>::Type temp("RightFall");
}

RightFall::RightFall() : Publisher("RightFall")
{

}


int RightFall::Execute() {
  MotionActionMessage msg;
  msg.set_command("rightFall.kme");
  publish(&msg,"motion");
  return 0;
}

void RightFall::UserInit () {
  _com->get_message_queue()->add_publisher(this);
}

std::string RightFall::GetName () {
	return "RightFall";
}
