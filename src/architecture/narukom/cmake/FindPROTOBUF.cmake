message(STATUS "Protocol Buffers")

if( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${RobotPlatform}/${RobotPlatform}-protobuf.cmake)
else( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-protobuf.cmake)
endif( "${TargetPlatform}" STREQUAL "robot")

SET (ProtobufIncludeDir ${ExternalDir}/protobuf/src/)



