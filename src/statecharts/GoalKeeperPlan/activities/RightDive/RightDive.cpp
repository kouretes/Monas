
#include "RightDive.h"

namespace {
    ActivityRegistrar<RightDive>::Type temp("RightDive");
}

int RightDive::Execute() {
	/* null */
	return 0;
}

void RightDive::UserInit () {
	;
}

std::string RightDive::GetName () {
	return "RightDive";
}
	