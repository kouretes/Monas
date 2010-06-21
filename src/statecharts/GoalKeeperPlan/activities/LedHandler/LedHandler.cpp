
#include "LedHandler.h"

namespace {
    ActivityRegistrar<LedHandler>::Type temp("LedHandler");
}

int LedHandler::Execute() {
	/* null */
	return 0;
}

void LedHandler::UserInit () {
	;
}

std::string LedHandler::GetName () {
	return "LedHandler";
}
	