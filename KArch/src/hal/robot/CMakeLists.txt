message ( STATUS "" )
message ( STATUS " _..::: Configuring Robot Abstraction Layer :::.._ " )
message ( STATUS "" )



set ( RobotPlatform nao CACHE STRING
        "Select robotic platform. Available platforms are nao and aibo" )

if ( "x${RobotPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "RobotPlatform can't be empty! Select an available platform: nao or aibo" )
elseif ( "${RobotPlatform}" STREQUAL "nao" )
    set ( NaoQiVersion 1.3.17 CACHE STRING
            "Set the NaoQi version" )
    if ( "x${NaoQiVersion}x" STREQUAL "xx" )
        message ( FATAL_ERROR "NaoQiVersion must be set" )
    endif ( "x${NaoQiVersion}x" STREQUAL "xx" )
    add_subdirectory ( generic_nao )
    add_subdirectory ( "nao-${NaoQiVersion}" )
elseif ( "${RobotPlatform}" STREQUAL "aibo" )
    add_subdirectory ( aibo )
else ( "x${RobotPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "Please select a valid platform: nao or aibo"  )
endif ( "x${RobotPlatform}x" STREQUAL "xx" )


