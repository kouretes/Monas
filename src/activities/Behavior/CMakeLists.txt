include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 
use(NAOQI-PLUGINS-TOOLS)


set ( ActivityName Behavior )

set ( ActivitySrcs Behavior.cpp ../../messages/motion.pb.cc 
                              ../../messages/VisionObservations.pb.cc 
                              ../../messages/SensorsMessage.pb.cc )

include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)

target_link_libraries ( ${ActivityName} NaoQiVer )

