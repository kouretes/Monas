message( STATUS "Pthread")

if( "${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${RobotPlatform}/${RobotPlatform}-pthread.cmake)
else("${TargetPlatform}" STREQUAL "robot")
  include(${ConfigDir}/${TargetPlatform}/${TargetPlatform}-pthread.cmake)
endif( "${TargetPlatform}" STREQUAL "robot")