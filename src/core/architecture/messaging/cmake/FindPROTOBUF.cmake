message(STATUS "Protocol Buffers")

if( NOT "x${TargetRobot}x" STREQUAL "xnonex")
  include(${ConfigDir}/robot/${TargetRobot}/${TargetRobot}-protobuf.cmake)
else( NOT "x${TargetRobot}x" STREQUAL "xnonex")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-protobuf.cmake)
endif( NOT "x${TargetRobot}x" STREQUAL "xnonex")



