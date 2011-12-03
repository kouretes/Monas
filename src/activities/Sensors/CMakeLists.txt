if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
use(NAOQI-PLUGINS-TOOLS)


set ( ActivityName Sensors)

set ( ActivitySrcs  Sensors.cpp )


if (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_ON ")
else (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_OFF ")
endif (KROBOT_IS_REMOTE)


include ( ../activTemplate.cmake )

STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
