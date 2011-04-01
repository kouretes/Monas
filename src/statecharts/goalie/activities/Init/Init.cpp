
#include "Init.h"

namespace {
    ActivityRegistrar<Init>::Type temp("Init");
}

int Init::Execute() {
	/*  */
	return 0;
}

void Init::UserInit () {
	;
}

std::string Init::GetName () {
	return "Init";
}
	