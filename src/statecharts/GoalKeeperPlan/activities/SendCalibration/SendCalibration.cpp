
#include "SendCalibration.h"

namespace {
    ActivityRegistrar<SendCalibration>::Type temp("SendCalibration");
}

int SendCalibration::Execute() {
	/* null */
	return 0;
}

void SendCalibration::UserInit () {
	;
}

std::string SendCalibration::GetName () {
	return "SendCalibration";
}
	