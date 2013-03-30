
message ( STATUS "" )
message ( STATUS " _..::: Configuring Sources :::.._ " )
message ( STATUS "" )

include_directories ( AFTER ../external/protobuf/src/ )



set ( LibBuildType STATIC CACHE STRING
        "Choose build type for internal libraries. Available options are STATIC, SHARED and MODULE" )

if ( "x${LibBuildType}x" STREQUAL "xx" )
  message ( FATAL_ERROR "LibBuildType is not set!" )
elseif ( (NOT "${LibBuildType}" STREQUAL "STATIC") AND
         (NOT "${LibBuildType}" STREQUAL "SHARED") AND
         (NOT "${LibBuildType}" STREQUAL "MODULE") )
  message ( FATAL_ERROR "LibBuildType should be one of the following: STATIC, SHARED or MODULE" )
endif ( "x${LibBuildType}x" STREQUAL "xx" )

add_subdirectory (./core/) #Monas Core
add_subdirectory ( ./messages/ )
add_subdirectory ( ./robocup/ )
#User components
add_subdirectory ( ./activities/ )
add_subdirectory ( ./providers/ )
add_subdirectory ( ./statecharts/ )

if( ${TargetRobot} STREQUAL "none")
	add_subdirectory ( ./GUITools/ )
endif( ${TargetRobot} STREQUAL "none")

#Call function that must be defined somewhere inside hal for proper linking

Monas_Finalize()


option ( BuildTests "Compile testing code" )




if ( BuildTests )
  add_subdirectory ( ./testing/ )
endif ( BuildTests )
