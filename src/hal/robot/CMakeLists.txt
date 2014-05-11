message ( STATUS "" )
message ( STATUS " _..::: Configuring Robot Abstraction Layer :::.._ " )
message ( STATUS "" )


if ( "x${TargetRobot}x" STREQUAL "xnonex" )
    message ( STATUS "No robot defined, skipping robot hal"  )
else("x${TargetRobot}x" STREQUAL "xnonex")
	if( IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${TargetRobot} )
		message(STATUS "Using folder: " ${TargetRobot})
		add_subdirectory(${TargetRobot})
	else()
		message ( FATAL_ERROR "Please select a valid TargetRobot:" ${TargetRobot}  )
	endif()
endif("x${TargetRobot}x" STREQUAL "xnonex")




