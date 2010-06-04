message(STATUS "External Dependencies")
find_package(PROTOBUF)

#message("Search for probobuf dependencies")
find_package(PTHREAD)
find_package(ZLIB)

#message(INFO "Searching Boost libraries")
find_package(BOOST)
find_package(UDT)


#message(INFO "Search for tinxyxml")
find_package(TINYXML)
