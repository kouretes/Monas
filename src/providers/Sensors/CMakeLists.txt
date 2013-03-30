if ( NOT (${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )



set ( ProviderName Sensors)

set ( ProviderSrcs  Sensors.cpp )




include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
