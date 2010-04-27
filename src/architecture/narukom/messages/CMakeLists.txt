project(narukom_messages)
find_package(PROTOBUF)
set(NARUKOM_MESSAGES_SRCS Basic.pb.h Basic.pb.cc)

add_library(narukom_messages ${NARUKOM_MESSAGES_SRCS})

target_link_libraries(${PROTOBUF_LIBRARIES})
