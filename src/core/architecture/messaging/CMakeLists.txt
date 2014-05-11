message ( STATUS "" )
message ( STATUS " _..::: Configuring Messaging :::.._ " )
message ( STATUS "" )

SET(MESSAGING_SRCS
   Blackboard.hpp
   Blackboard.cpp
   msg.h
   TopicTree.hpp
   TopicTree.cpp
   EndPoint.hpp
   EndPoint.cpp
   MessageHub.cpp
   MessageHub.hpp
   MessageBuffer.cpp
   MessageBuffer.hpp

)

set(NETWORK_SRCS
	network/multicastpoint.hpp
	network/multicastpoint.cpp
	network/rawPacketDePacket.h
	network/rawPacketDePacket.cpp
	network/ptimeserialize.hpp
	network/msgentryserialize.hpp
)

#setting default module path for cmake finding .cmake files
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake {CMAKE_MODULE_PATH})
#Setting the configuration Directory where all the .cmake are stored it's the same as CMAKE_MODULE_PATH
set (ConfigDir ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/external_dependencies.cmake)

#include the specific configuration for the target platform
#include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-config.cmake)


if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions("-g")
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

include_directories( ${NarukomDir}
		     ${ProtobufIncludeDir}
		     ${BoostIncludeDir}
		     )
add_library(messaging ${LibBuildType}   ${MESSAGING_SRCS}   ${NETWORK_SRCS} ) # ${NARUKOM_SYSTEM_SRCS}  ${SRCS}  ${NETWORK_SRCS} 

install ( TARGETS messaging
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )


target_link_libraries(messaging ${ProtobufLibrary} )
