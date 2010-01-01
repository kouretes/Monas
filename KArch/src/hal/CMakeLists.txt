message ( STATUS "" )
message ( STATUS " _..::: Configuring Hardware Abstraction Layer :::.._ " )
message ( STATUS "" )


set ( TargetPlatform linux CACHE STRING 
        "select the targeted platform, available platforms are linux, windows, and robot" )

set ( CMAKE_INSTALL_PREFIX "${CMAKE_HOME_DIRECTORY}/binaries/${TargetPlatform}" CACHE PATH "Select the install directory" FORCE )

if ( "x${TargetPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "TargetPlatform can't be empty! Select an available platform: linux, windows or robot" )
elseif ( "${TargetPlatform}" STREQUAL "windows" )
    message ( FATAL_ERROR "Not supported yet!!!" )
elseif ( "${TargetPlatform}" STREQUAL "linux" )
    add_subdirectory ( generic_linux )
    add_subdirectory ( target_linux )
elseif ( "${TargetPlatform}" STREQUAL "robot" )
    add_subdirectory ( generic_linux ) #if robotplatform == nao || aibo 
    add_subdirectory ( robot )
else ( "x${TargetPlatform}x" STREQUAL "xx" )
    message ( FATAL_ERROR "Please select a valid platform: linux, windows or robot"  )
endif ( "x${TargetPlatform}x" STREQUAL "xx" )


