message(STATUS "External Dependencies")


#message("Search for probobuf dependencies")
#find_package(PTHREAD)
find_package(ZLIB)


find_package(PROTOBUF)

#message(INFO "Searching Boost libraries")
find_package(Boost COMPONENTS date_time thread system)

