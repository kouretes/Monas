
#include "SendCalibration.h"

namespace {
    ActivityRegistrar<SendCalibration>::Type temp("SendCalibration");
}

SendCalibration::SendCalibration() : Publisher("SendCalibration")
{
  ;
}


int SendCalibration::Execute() {
  CalibrateCam msg;
  msg.set_status(0);
  publish(&msg,"vision");
  return 0;
}

void SendCalibration::UserInit () {
  _com->get_message_queue()->add_publisher(this);
}

std::string SendCalibration::GetName () {
	return "SendCalibration";
}
