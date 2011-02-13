
#include "Stand.h"

namespace {
    ActivityRegistrar<Stand>::Type temp("Stand");
}

int Stand::Execute() {
	/* null */
	return 0;
}

void Stand::UserInit () {
	;
}

std::string Stand::GetName () {
	return "Stand";
}
	