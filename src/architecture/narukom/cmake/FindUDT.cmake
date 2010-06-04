message(STATUS "UDT")

if( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${RobotPlatform}/${RobotPlatform}-udt.cmake)
else( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-udt.cmake)
endif( "${TargetPlatform}" STREQUAL "robot")