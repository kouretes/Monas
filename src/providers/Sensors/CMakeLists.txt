if ( NOT (${TargetRobot} STREQUAL "nao" ) OR KROBOT_IS_REMOTE) 
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" )  OR KROBOT_IS_REMOTE)



set ( ProviderName Sensors)

set ( ProviderSrcs  Sensors.cpp )




include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
