
#include "TrackBall.h"

namespace {
    ActivityRegistrar<TrackBall>::Type temp("TrackBall");
}

int TrackBall::Execute() {
	/* null */
	return 0;
}

void TrackBall::UserInit () {
	;
}

std::string TrackBall::GetName () {
	return "TrackBall";
}
	