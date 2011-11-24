if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )


set ( ActivityName ObstacleAvoidance)

set ( ActivitySrcs  ObstacleAvoidance.cpp
)
if (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_ON ")
else (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_OFF ")
endif (KROBOT_IS_REMOTE)
include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS OPENCV)
