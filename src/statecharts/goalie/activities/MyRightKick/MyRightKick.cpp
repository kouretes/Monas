
#include "MyRightKick.h"

namespace {
    ActivityRegistrar<MyRightKick>::Type temp("MyRightKick");
}

int MyRightKick::Execute() {
	/*  */
	return 0;
}

void MyRightKick::UserInit () {
	;
}

std::string MyRightKick::GetName () {
	return "MyRightKick";
}
	