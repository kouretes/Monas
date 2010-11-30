if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

include( ../../hal/robot/nao-1.6.0/bootstrap.cmake )

set ( ActivityName MotionController )

set ( ActivitySrcs  MotionController.cpp
                    KmeManager.cpp
                    KmeAction.cpp
                    XarManager.cpp
                    XarAction.cpp
)

include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
if(Webots)
  message ( STATUS " _..::: Configuring MotionController for Webots :::.._ " )
  add_definitions(" -DWEBOTS ")
endif(Webots)

add_subdirectory ( SpecialActions )

set ( ActivityLibraries ${ActivityLibraries} MotionController ${SpecialActionLibraries} PARENT_SCOPE )
