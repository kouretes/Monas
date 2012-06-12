if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

set ( ActivityName OpenChallenge2012 )

set ( ActivitySrcs  OpenChallenge2012.cpp
                    XarManager.cpp
                    XarAction.cpp
)

include ( ../activTemplate.cmake )
STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
if(Webots)
  message ( STATUS " _..::: Configuring OpenChallenge2012 for Webots :::.._ " )
  add_definitions(" -DWEBOTS ")
endif(Webots)

add_subdirectory ( SpecialActions )


set ( ActivityLibraries ${ActivityLibraries} OpenChallenge2012 ${SpecialActionLibraries} PARENT_SCOPE )
