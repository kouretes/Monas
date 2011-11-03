
#include "MyLeftKick.h"

namespace {
    ActivityRegistrar<MyLeftKick>::Type temp("MyLeftKick");
}

int MyLeftKick::Execute() {
	/*  */
	return 0;
}

void MyLeftKick::UserInit () {
	;
}

std::string MyLeftKick::GetName () {
	return "MyLeftKick";
}
	