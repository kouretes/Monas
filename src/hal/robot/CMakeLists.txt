message ( STATUS "" )
message ( STATUS " _..::: Configuring Robot Abstraction Layer :::.._ " )
message ( STATUS "" )


if ( "x${TargetRobot}x" STREQUAL "xnonex" OR "${TargetRobot}" STREQUAL "host")
    message ( STATUS "No robot defined, skipping robot hal"  )
else("x${TargetRobot}x" STREQUAL "xnonex" OR ${TargetRobot} STREQUAL "host" )
	if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TargetRobot} )
		message(STATUS "Using folder: " ${TargetRobot})
		add_subdirectory(${TargetRobot})
	else(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TargetRobot} )
		message ( FATAL_ERROR "Please select a valid TargetRobot:" ${TargetRobot}  )
	endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TargetRobot} )
endif("x${TargetRobot}x" STREQUAL "xnonex" OR ${TargetRobot} STREQUAL "host" )




