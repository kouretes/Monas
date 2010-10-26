if ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )
    return()
endif ( NOT ( ${TargetPlatform} STREQUAL "robot" AND ${RobotPlatform} STREQUAL "nao" ) )

include( ../../hal/robot/nao-1.6.0/bootstrap.cmake )
include_directories( ../../../external/PracticalSocket)

set ( ActivityName Vision )

set ( ActivitySrcs  KImageExtractor.cpp
                    KSegmentator.cpp
                    Vision.cpp
		    KCameraTransformation.cpp ../../../external/PracticalSocket/PracticalSocket.cpp
)

include ( ../activTemplate.cmake )


use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS LIBCORE LIBVISION OPENCV)
add_executable(KMat-testing KMat-testing.cpp KMat.h)
