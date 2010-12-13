message ( STATUS "" )
message ( STATUS " _..::: Configuring Robot Abstraction Layer :::.._ " )
message ( STATUS "" )


if ( "x${RobotPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "RobotPlatform can't be empty! Select an available platform: nao or aibo" )
elseif ( "${RobotPlatform}" STREQUAL "nao" )

    
    if ( "x${NaoQiVersion}x" STREQUAL "xx" )
        message ( FATAL_ERROR "NaoQiVersion must be set" )
    endif ( "x${NaoQiVersion}x" STREQUAL "xx" )
    set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/naoqi_${NaoQiVersion}" CACHE PATH "Select the install directory" FORCE )
    STRING(REPLACE "." "_" NaoQiVer ${NaoQiVersion})
    add_definitions(" -DNAOQI_${NaoQiVer} ")
    add_subdirectory ( generic_nao )
    add_subdirectory ( "nao-${NaoQiVersion}" )
    

elseif ( "${RobotPlatform}" STREQUAL "aibo" )

    set ( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/aibo" CACHE PATH "Select the install directory" FORCE )
    add_subdirectory ( aibo )

else ( "x${RobotPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "Please select a valid platform: nao or aibo"  )
endif ( "x${RobotPlatform}x" STREQUAL "xx" )


