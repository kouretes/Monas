if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
use(NAOQI-PLUGINS-TOOLS)


set ( ProviderName ImageExtractor)

set ( ProviderSrcs  ImageExtractor.cpp ImageExtractor.h )


if (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_ON ")
else (KROBOT_IS_REMOTE)
  add_definitions(" -DKROBOT_IS_REMOTE_OFF ")
endif (KROBOT_IS_REMOTE)


include ( ../provTemplate.cmake )

STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")

use_lib(${ProviderName} ALCOMMON LIBVISION)
