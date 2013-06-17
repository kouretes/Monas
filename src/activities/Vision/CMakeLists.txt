if ( NOT ( ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )

set ( ActivityName Vision )

set ( ActivitySrcs  Vision.cpp
                    Vision.h
		    VisionImageProcessing.cpp
		    KCameraTransformation.cpp

)
include ( ../activTemplate.cmake )


qi_use_lib(${ActivityName} ALCOMMON ALVISION)
