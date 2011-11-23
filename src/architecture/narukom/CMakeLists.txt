#PROJECT(narukom)

cmake_minimum_required(VERSION 2.6.4)

#setting the top level narukom directory
set (NarukomDir ${CMAKE_CURRENT_SOURCE_DIR})

#setting default module path for cmake finding .cmake files
set (CMAKE_MODULE_PATH ${NarukomDir}/cmake)
#Setting the configuration Directory where all the .cmake are stored it's the same as CMAKE_MODULE_PATH
set (ConfigDir ${NarukomDir}/cmake)


#if no targetplatform has been set then the default is linux
if ( "x${TargetPlatform}x" STREQUAL "xx")
  set(TargetPlatform "linux" CACHE PATH "")
endif( "x${TargetPlatform}x" STREQUAL "xx")



#The .cmake where all the narukom sources files are defined
include(${ConfigDir}/narukom_sources.cmake)

include(${ConfigDir}/external_dependencies.cmake)


#include the specific configuration for the target platform
#include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-config.cmake)

add_definitions(${ExternalDefinitions}
	  "-Wextra"
)

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions("-g")
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

include_directories( ${NarukomDir}
		     ${ProtobufIncludeDir}
		     ${BoostIncludeDir}
		     )
add_library(narukom ${LibBuildType} narukom.cpp ${NARUKOM_SYSTEM_SRCS}  ${NARUKOM_PUB_SUB_SRCS}  ${NARUKOM_NETWORK_SRCS}  ${NARUKOM_MESSAGES_SRCS}  )

install ( TARGETS narukom
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )


set ( TargetLinkLibraries ${TargetLinkLibraries} CACHE INTERNAL "" )

target_link_libraries(narukom ${ProtobufLibrary} )
