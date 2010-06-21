
#include "Stand.h"

namespace {
    ActivityRegistrar<Stand>::Type temp("Stand");
}

Stand::Stand() : Publisher("Stand") {
    ;
}


int Stand::Execute() {
  MotionWalkMessage msg;
  msg.set_command("walkTo");
  msg.add_parameter(0.01);
  msg.add_parameter(0.0);
  msg.add_parameter(0.0);
  publish(&msg,"motion");
}

void Stand::UserInit () {
	_com->get_message_queue()->add_publisher(this);
}

std::string Stand::GetName () {
	return "Stand";
}
