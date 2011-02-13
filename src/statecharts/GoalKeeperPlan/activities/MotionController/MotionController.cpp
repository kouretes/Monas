
#include "MotionController.h"

namespace {
    ActivityRegistrar<MotionController>::Type temp("MotionController");
}

int MotionController::Execute() {
	/* null */
	return 0;
}

void MotionController::UserInit () {
	;
}

std::string MotionController::GetName () {
	return "MotionController";
}
	