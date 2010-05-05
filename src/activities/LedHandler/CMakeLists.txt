set ( ActivityName LedHandler )

set ( ActivitySrcs LedHandler.cpp ../../messages/Gamecontroller.pb.cc )

include ( ../activTemplate.cmake )


target_link_libraries ( ${ActivityName} NaoQiDep )
use_lib(${ActivityName}  LIBCORE TOOLS LIBCORE ALCOMMON)
