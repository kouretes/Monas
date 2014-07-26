if ( NOT (${TargetRobot} STREQUAL "nao" )  OR KROBOT_IS_REMOTE)
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) OR KROBOT_IS_REMOTE)



set ( ProviderName ImageExtractor)

set ( ProviderSrcs  ImageExtractor.cpp ImageExtractor.h )





include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName} ALCOMMON ALVISION)
