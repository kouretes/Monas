include( ../../hal/robot/nao-1.6.0/bootstrap.cmake ) 
use(NAOQI-PLUGINS-TOOLS)

set ( ActivityName MotionController )

set ( ActivitySrcs MotionController.cpp ../../messages/motion.pb.cc ) 

include ( ../activTemplate.cmake )

use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
