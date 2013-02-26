if ( NOT ( ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )

set ( ProviderName LedHandler )

set ( ProviderSrcs LedHandler.cpp )

include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName}  LIBCORE ALTOOLS ALCOMMON)

target_link_libraries ( ${ProviderName} NaoQiDep )

