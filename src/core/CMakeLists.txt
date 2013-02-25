message ( STATUS "" )
message ( STATUS " _..::: Configuring Core :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./architecture/ )
add_subdirectory ( ./elements/ )


set ( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/binaries/${TargetPlatform}" CACHE PATH "Select the install directory" FORCE )

