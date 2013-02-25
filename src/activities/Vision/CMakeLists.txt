if ( NOT ( ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )

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


qi_use_lib(${ActivityName} ALCOMMON ALVISION)
