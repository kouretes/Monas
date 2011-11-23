if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

include_directories( ../../../external/PracticalSocket)

set ( ActivityName Vision )

set ( ActivitySrcs  KSegmentator.cpp
					KSegmentator.h
					Vision.cpp
                    Vision.h
		    VisionImageProcessing.cpp
		    KCameraTransformation.cpp ../../../external/PracticalSocket/PracticalSocket.cpp

)
include ( ../activTemplate.cmake )
STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
add_definitions(" -DNAOQI_${NaoQiVer} ")



use_lib(${ActivityName} ALCOMMON LIBVISION)
