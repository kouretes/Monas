if ( NOT (${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )



set ( ProviderName ImageExtractor)

set ( ProviderSrcs  ImageExtractor.cpp ImageExtractor.h )





include ( ../provTemplate.cmake )

qi_use_lib(${ProviderName} ALCOMMON LIBVISION)
