message( "Searching Google's Protocol Buffers")
find_package(PROTOBUF)

message("Search for probobuf dependencies")
find_package(PTHREAD)
find_package(ZLIB)

message(INFO "Searching Boost libraries")
find_package(BOOST)