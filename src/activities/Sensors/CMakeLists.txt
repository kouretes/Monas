if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )


set ( ActivityName Sensors)

set ( ActivitySrcs  Sensors.cpp 
)
include( ../../hal/robot/nao-${NaoQiVersion}/bootstrap.cmake ) 
STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")


include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
