if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

set ( ActivityName HeadBehavior )

set ( ActivitySrcs  HeadBehavior.cpp 
)

include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)

target_link_libraries ( ${ActivityName} NaoQiDep )

