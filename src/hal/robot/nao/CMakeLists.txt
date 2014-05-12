message ( STATUS "" )
message ( STATUS " _..::: Configuring Robot ${TargetRobot} :::.._ " )
message ( STATUS "" )

if (NOT  "x${TargetPlatform}x" STREQUAL "xlinuxx" )
	message ( FATAL_ERROR "TargetPlatform  for nao robots must be set to linux " )
endif (NOT  "x${TargetPlatform}x" STREQUAL "xlinuxx" )

set ( NaoQiVersion 1.12.5 CACHE STRING "NaoQi version" )

if ( "x${NaoQiVersion}x" STREQUAL "xx" )
	message ( FATAL_ERROR "NaoQiVersion must be set" )
endif ( "x${NaoQiVersion}x" STREQUAL "xx" )

#Generate NAOQI_SOMEVERSION definition
STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
Monas_global_add_definitions(-DNAOQI_${NaoQiVer})
#Also simply define NAOQI
Monas_global_add_definitions(-DNAOQI=${NaoQiVersion})
add_subdirectory ( generic_nao )
add_subdirectory ( "nao-${NaoQiVersion}" )
if( "${SDK_ARCH}" STREQUAL "nao-geode" )
  set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/naoqi_${NaoQiVersion}_cross" CACHE PATH "Select the install directory" FORCE )
elseif( "${SDK_ARCH}" STREQUAL "linux" )
  set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/naoqi_${NaoQiVersion}_linux" CACHE PATH "Select the install directory" FORCE )
endif( "${SDK_ARCH}" STREQUAL "nao-geode" )



