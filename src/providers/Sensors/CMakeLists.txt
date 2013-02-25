if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${TargetRobot} STREQUAL "nao" ) )



set ( ProviderName Sensors)

set ( ProviderSrcs  Sensors.cpp )




include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
