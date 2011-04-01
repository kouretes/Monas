
#include "NoPlay.h"

namespace {
    ActivityRegistrar<NoPlay>::Type temp("NoPlay");
}

int NoPlay::Execute() {
	/*  */
	return 0;
}

void NoPlay::UserInit () {
	;
}

std::string NoPlay::GetName () {
	return "NoPlay";
}
	