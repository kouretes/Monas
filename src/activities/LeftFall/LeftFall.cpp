
#include "LeftFall.h"

namespace {
    ActivityRegistrar<LeftFall>::Type temp("LeftFall");
}

LeftFall::LeftFall() : Publisher("LeftFall")
{
  ;
}


int LeftFall::Execute() {
  MotionActionMessage msg;
  msg.set_command("leftFall.kme");
  publish(&msg,"motion");
  return 0;
}

void LeftFall::UserInit () {
  _com->get_message_queue()->add_publisher(this);
}

std::string LeftFall::GetName () {
	return "LeftFall";
}
