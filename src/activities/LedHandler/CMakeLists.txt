include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 
use(NAOQI-PLUGINS-TOOLS)

set ( ActivityName LedHandler )

set ( ActivitySrcs LedHandler.cpp ../../messages/Gamecontroller.pb.cc )

include ( ../activTemplate.cmake )

use_lib(${ActivityName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${ActivityName} NaoQiDep )

