message ( STATUS "" )
message ( STATUS " _..::: Configuring NaoQi ver 1.10.10 :::.._ " )
message ( STATUS "" )


include("${CMAKE_CURRENT_SOURCE_DIR}/bootstrap.cmake")
use(NAOQI-PLUGINS-TOOLS)


add_library ( NaoQiDep  ${LibBuildType}
                        earLed.cpp
                        HeadAngleSensor.cpp
                        KImageExtractor.cpp
)

use_lib( NaoQiDep ALCOMMON ALMATH LIBCORE TOOLS ALVALUE BOOST)

set ( TargetLinkLibraries ${TargetLinkLibraries} NaoQiDep)

install ( TARGETS NaoQiDep
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

add_subdirectory ( krobot )
