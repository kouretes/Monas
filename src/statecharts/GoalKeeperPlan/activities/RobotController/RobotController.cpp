
#include "RobotController.h"

namespace {
    ActivityRegistrar<RobotController>::Type temp("RobotController");
}

int RobotController::Execute() {
	/* null */
	return 0;
}

void RobotController::UserInit () {
	;
}

std::string RobotController::GetName () {
	return "RobotController";
}
	