message ( STATUS "" )
message ( STATUS " _..::: Configuring Hardware Abstraction Layer :::.._ " )
message ( STATUS "" )


set(HAL_SRCS
      SystemThread.hpp
      SystemThreadPool.hpp
      SystemMutex.hpp
      SystemCondVar.hpp
      SystemClock.hpp
)

add_custom_target(Monas-hal SOURCES ${HAL_SRCS}  )

set ( TargetPlatform "" CACHE STRING
        "select the targeted platform, available platforms are linux, windows" )

set ( TargetRobot "none" CACHE STRING
        "Select robotic platform. Available platforms are nao , host and none" )

if ( "x${TargetPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "TargetPlatform can't be empty! Select an available platform: linux " )
endif ( "x${TargetPlatform}x" STREQUAL "xx" )


if ( "x${TargetRobot}x" STREQUAL "xx" )
    message ( FATAL_ERROR "TargetPlatform can't be empty! Select an available robot:  nao , host , none" )
endif ( "x${TargetRobot}x" STREQUAL "xx" )

add_subdirectory(generic_${TargetPlatform})


if ( "${TargetRobot}" STREQUAL "none" OR "${TargetRobot}" STREQUAL "host" )
	add_subdirectory(target_${TargetPlatform})
endif ( "${TargetRobot}" STREQUAL "none" OR "${TargetRobot}" STREQUAL "host" )
add_subdirectory(robot)





