
#include "Scan.h"

namespace {
    ActivityRegistrar<Scan>::Type temp("Scan");
}

int Scan::Execute() {
	/*  */
	return 0;
}

void Scan::UserInit () {
	;
}

std::string Scan::GetName () {
	return "Scan";
}
	