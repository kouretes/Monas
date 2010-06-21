
#include "LeftDive.h"

namespace {
    ActivityRegistrar<LeftDive>::Type temp("LeftDive");
}

int LeftDive::Execute() {
	/* null */
	return 0;
}

void LeftDive::UserInit () {
	;
}

std::string LeftDive::GetName () {
	return "LeftDive";
}
	