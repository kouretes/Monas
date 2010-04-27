message(INFO "UTISLS ${CUR_DIR}")

add_executable(ping_local pingpong_local.cpp pinger.cpp ponger.cpp scorekeeper.cpp echo.cpp pingpong.pb.cc)
target_link_libraries(ping_local narukomlib )
