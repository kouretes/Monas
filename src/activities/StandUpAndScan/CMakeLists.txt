
#if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
#    return()
#endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

#include( ../../hal/robot/nao-1.6.0/bootstrap.cmake )

set ( ActivityName StandUpAndScan )

set ( ActivitySrcs  StandUpAndScan.cpp )

include ( ../activTemplate.cmake )

#qi_use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)	
	