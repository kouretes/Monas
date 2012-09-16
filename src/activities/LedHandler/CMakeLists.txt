if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

set ( ActivityName LedHandler )

set ( ActivitySrcs LedHandler.cpp )

include ( ../activTemplate.cmake )

STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")


qi_use_lib(${ActivityName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${ActivityName} NaoQiDep )

