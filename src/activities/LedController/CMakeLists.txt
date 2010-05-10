if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

set ( ActivityName LedController )

set ( ActivitySrcs LedController.cpp )

include ( ../activTemplate.cmake )


target_link_libraries ( ${ActivityName} NaoQiDep )
