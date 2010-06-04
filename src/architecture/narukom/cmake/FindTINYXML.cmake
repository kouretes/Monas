message(STATUS "Tinyxml")

if( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${RobotPlatform}/${RobotPlatform}-tinyxml.cmake)
else( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-tinyxml.cmake)
endif( "${TargetPlatform}" STREQUAL "robot")