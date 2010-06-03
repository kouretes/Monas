message(STATUS "Boost")


if( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${RobotPlatform}/${RobotPlatform}-boost.cmake)
else( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-boost.cmake)
endif( "${TargetPlatform}" STREQUAL "robot")


