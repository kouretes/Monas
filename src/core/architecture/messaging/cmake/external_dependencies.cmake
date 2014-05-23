message(STATUS "External Dependencies")

message(STATUS PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR})

message(STATUS "MODULE PATH:" ${CMAKE_MODULE_PATH} )

find_package(ZLIB REQUIRED)
#message("Search for probobuf dependencies")
find_package(PTHREAD)



find_package(PROTOBUF)

#message(INFO "Searching Boost libraries")
#find_package(Boost COMPONENTS date_time thread system)

