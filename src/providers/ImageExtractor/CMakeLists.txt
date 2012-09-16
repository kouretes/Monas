if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )



set ( ProviderName ImageExtractor)

set ( ProviderSrcs  ImageExtractor.cpp ImageExtractor.h )





include ( ../provTemplate.cmake )

STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")

qi_use_lib(${ProviderName} ALCOMMON LIBVISION)
