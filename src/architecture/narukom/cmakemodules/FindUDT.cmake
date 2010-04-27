message("------------------ UDT -------------")
#set ( TARGET_HOST  "adfas")

#EXEC_PROGRAM("pushd $ENV{ROBOCUP_HOME}/external/protobuf/")
#Message("------- ${CMAKE_CURRENT_SOURCE_DIR} ------------")
#SET( OUTPUT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/protobuf/" )
#SET( OUTPUT_ROOT_DIR_LIB "${_DIR}/modules/lib/" )
EXEC_PROGRAM( $ENV{ROBOCUP_HOME}/external/protobuf/configure )#ARGS "$ENV{ROBOCUP_HOME}/external/protobuf/build/"  )

EXEC_PROGRAM("/usr/bin/make"  ARGS " -f $ENV{ROBOCUP_HOME}/external/protobuf/Makefile" )
set (UDT_INCLUDE_DIR $ENV{ROBOCUP_HOME}/external/udt/)
set (UDT_LIBS_DIR $ENV{ROBOCUP_HOME}/external/udt/libudt.so)
message("proto_include is ${UDT_CFLAGS}")
message("proto_include is ${UDT_LIBS_DIR}")
