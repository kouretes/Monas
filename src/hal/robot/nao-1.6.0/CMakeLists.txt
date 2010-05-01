add_subdirectory ( krobot )

include("${CMAKE_CURRENT_SOURCE_DIR}/bootstrap.cmake")
use(NAOQI-PLUGINS-TOOLS)


add_library ( NaoQiVer earLed.cpp HeadAngleSensor.cpp )

use_lib( NaoQiVer ALCOMMON ALMATH LIBCORE TOOLS ALVALUE)

set ( TargetLinkLibraries ${TargetLinkLibraries} NaoQiVer) 
