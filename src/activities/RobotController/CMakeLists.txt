include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 
#use(NAOQI-PLUGINS-TOOLS)

set ( ActivityName RobotController )

set ( ActivitySrcs gamecontroller.cpp robotcontroller.cpp   ../../messages/Gamecontroller.pb.cc) 

include ( ../activTemplate.cmake )

use_lib(${ActivityName}  LIBCORE TOOLS LIBCORE BOOST TINYXML ALCOMMON ALVALUE)