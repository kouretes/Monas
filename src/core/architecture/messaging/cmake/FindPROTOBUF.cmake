message(STATUS "Protocol Buffers")

if(  "x${TargetRobot}x" STREQUAL "xnonex" OR "x${TargetRobot}x" STREQUAL "xhostx")
	include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-protobuf.cmake)
  
else(  "x${TargetRobot}x" STREQUAL "xnonex")
  include(${ConfigDir}/robot/${TargetRobot}/${TargetRobot}-protobuf.cmake)
endif(  "x${TargetRobot}x" STREQUAL "xnonex")



