message ( STATUS "" )
message ( STATUS " _..::: Configuring Sources :::.._ " )
message ( STATUS "" )

include_directories ( AFTER ./ )

add_subdirectory ( ./architecture/ )

#TODO comment: must be before target linux
add_subdirectory ( ./activities/ )
add_subdirectory ( ./hal/ )

#TODO
add_subdirectory ( ./testing/ )
