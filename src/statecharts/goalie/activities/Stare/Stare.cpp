
#include "Stare.h"

namespace {
    ActivityRegistrar<Stare>::Type temp("Stare");
}

int Stare::Execute() {
	/*  */
	return 0;
}

void Stare::UserInit () {
	;
}

std::string Stare::GetName () {
	return "Stare";
}
	