
#include "Sensors.h"

namespace {
    ActivityRegistrar<Sensors>::Type temp("Sensors");
}

int Sensors::Execute() {
	/* null */
	return 0;
}

void Sensors::UserInit () {
	;
}

std::string Sensors::GetName () {
	return "Sensors";
}
	