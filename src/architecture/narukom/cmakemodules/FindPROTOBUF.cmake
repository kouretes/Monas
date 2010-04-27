message("------------------ Google protocol buffers -------------")

#Act depending on the architecture
include(cmakemodules/${TARGET_PLAT}-protobuf.cmake)

SET (PROTOBUF_INCLUDE_DIR ${NARUKOM_DIR}/external/protobuf/src/)
#SET (PROTOBUF_DEFINITIONS "-pthread" "-lprotobuf" "-lz")
#EXEC_PROGRAM( ${CUR_DIR}/external/protobuf/configure )#ARGS "$ENV{ROBOCUP_HOME}/external/protobuf/build/"  )

#EXEC_PROGRAM("/usr/bin/make"  ARGS " -f $ENV{ROBOCUP_HOME}/external/protobuf/Makefile" )
#set (PROTOBUF_INCLUDE_DIR $ENV{ROBOCUP_HOME}/external/protobuf/)
#set (PROTOBUF_LIBS_DIR $ENV{ROBOCUP_HOME}/external/protobuf/src/.libs/)

#message("proto_include is ${PROTOBUF_CFLAGS}")
message("proto_include is ${PROTOBUF_INCLUDE_DIR}")


