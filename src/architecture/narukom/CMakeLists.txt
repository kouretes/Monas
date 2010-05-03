#PROJECT(Narukom)
#cmake_minimum_required(VERSION 2.6.4)
#set ( CMAKE_COLOR_MAKEFILE TRUE )


#Getting Current directory
set (NARUKOM_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set (TARGET_PLAT "nao")
#Path to find .cmake files

set (CMAKE_MODULE_PATH "${NARUKOM_DIR}/cmakemodules")

IF ( "${TARGET_PLAT}" STREQUAL "")
	set(TARGET_PLAT "generic")
ENDIF( "${TARGET_PLAT}" STREQUAL "")

#include("${TARGET_PLAT}-config.cmake")
include(narukom_srcs.cmake)
include(external_dependencies.cmake)

include_directories( ${NARUKOM_SYSTEM_INCLUDE_DIR}
    ${NARUKOM_PUB_SUB_INCLUDE_DIR}
    ${NARUKOM_NETWORK_INCLUDE_DIR}
    ${NARUKOM_MESSAGES_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIR}
    ${BOOST_INCLUDE_DIR}
    )
add_definitions(${TARGET_PLAT_DEFINITIONS})

#add_library(Narukom)

#set (NARUKOM_DEFINITIONS  ${PROTOBUF_DEFINITIONS} ${BOOST_DEFINITIONS})
#message (INFO ${NARUKOM_DEFINITIONS})
add_library(narukomlib SHARED narukom.cpp ${NARUKOM_SYSTEM_SRCS}  ${NARUKOM_PUB_SUB_SRCS}  ${NARUKOM_NETWORK_SRCS}
    ${NARUKOM_MESSAGES_SRCS}  )
link_libraries(${PROTOBUF_LIBRARIES} ${BOOST_LIBRARIES} ${ZLIB_LIBRARIES} ${PTHREAD_LIBRARIES})

target_link_libraries(narukomlib ${PROTOBUF_LIBRARIES} ${BOOST_LIBRARIES} ${ZLIB_LIBRARIES} ${PTHREAD_LIBRARIES})
#add_subdirectory(demo)

install ( TARGETS narukomlib
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static ) 
