message ( STATUS "" )
message ( STATUS " _..::: Configuring NaoQi ver 1.8.16 :::.._ " )
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

if( "${SDK_ARCH}" STREQUAL "nao-geode" )
  set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}_cross" CACHE PATH "Select the install directory" FORCE )
elseif( "${SDK_ARCH}" STREQUAL "linux" )
  set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}_linux" CACHE PATH "Select the install directory" FORCE )
endif( "${SDK_ARCH}" STREQUAL "nao-geode" )

install ( TARGETS NaoQiDep
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

add_subdirectory ( krobot )
