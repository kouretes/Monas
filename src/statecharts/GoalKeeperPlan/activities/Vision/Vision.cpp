
#include "Vision.h"

namespace {
    ActivityRegistrar<Vision>::Type temp("Vision");
}

int Vision::Execute() {
	/* null */
	return 0;
}

void Vision::UserInit () {
	;
}

std::string Vision::GetName () {
	return "Vision";
}
	