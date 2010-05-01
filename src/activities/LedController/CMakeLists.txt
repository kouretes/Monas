set ( ActivityName LedController )

set ( ActivitySrcs LedController.cpp )

include ( ../activTemplate.cmake )


target_link_libraries ( ${ActivityName} NaoQiVer )
