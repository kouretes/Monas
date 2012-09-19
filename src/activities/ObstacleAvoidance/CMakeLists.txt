#if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
#    return()
#endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )


set ( ActivityName ObstacleAvoidance)

set ( ActivitySrcs  ObstacleAvoidance.cpp
)

include ( ../activTemplate.cmake )

#qi_use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS OPENCV)
