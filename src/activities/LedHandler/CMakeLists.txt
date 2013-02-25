if ( NOT ( ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )

set ( ActivityName LedHandler )

set ( ActivitySrcs LedHandler.cpp )

include ( ../activTemplate.cmake )

qi_use_lib(${ActivityName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${ActivityName} NaoQiDep )

