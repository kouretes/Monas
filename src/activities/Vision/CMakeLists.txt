include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 
use(NAOQI-PLUGINS-TOOLS)

set ( ActivityName Vision )

set ( ActivitySrcs  KImageExtractor.cpp 
                    KSegmentator.cpp
                    Vision.cpp
                    ../../messages/VisionObservations.pb.cc 
) 

include ( ../activTemplate.cmake )


use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS LIBCORE LIBVISION OPENCV)
