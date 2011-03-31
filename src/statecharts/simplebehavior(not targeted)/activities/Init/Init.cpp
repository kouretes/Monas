
#include "Init.h"

namespace {
    ActivityRegistrar<Init>::Type temp("Init");
}

int Init::Execute() {
	/*  */
	std::cout<<"STATE INIT executed "<<std::endl;
	return 0;
}

void Init::UserInit () {
	;
}

std::string Init::GetName () {
	return "Init";
}
	
