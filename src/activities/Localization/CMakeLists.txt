
if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )


include_directories( ../../../external/PracticalSocket)

set ( ActivityName Localization )

set ( ActivitySrcs  Localization.h Localization.cpp BallFilter.h BallFilter.cpp
newran/newran.h newran/newran.cpp newran/include.h newran/boolean.h newran/extreal.h newran/extreal.cpp newran/myexcept.cpp newran/myexcept.h
KLocalization.h KLocalization.cpp ../../../external/PracticalSocket/PracticalSocket.cpp
)

include ( ../activTemplate.cmake )

if(Webots)
  message ( STATUS " _..::: Configuring Localization for Webots :::.._ " )
  add_definitions(" -DWEBOTS ")
endif(Webots)
