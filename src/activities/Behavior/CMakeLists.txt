if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )


include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 


set ( ActivityName Behavior )

set ( ActivitySrcs Behavior.cpp ../../messages/motion.pb.cc 
                              ../../messages/VisionObservations.pb.cc 
                              ../../messages/SensorsMessage.pb.cc )

include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)

target_link_libraries ( ${ActivityName} NaoQiDep )

