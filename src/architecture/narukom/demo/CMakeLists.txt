message (STATUS "Building Demos")
add_definitions("-g")
add_executable(ping_local pingpong_local.cpp pinger.cpp ponger.cpp scorekeeper.cpp echo.cpp pingpong.pb.cc)
target_link_libraries(ping_local narukom )

add_executable(synchro_test synchronization_test.cpp fast.cpp slow.cpp ratio.cpp sync.pb.cc)
target_link_libraries( synchro_test narukom)