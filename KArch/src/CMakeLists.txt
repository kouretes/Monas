message ( STATUS "" )
message ( STATUS " _..::: Configuring Sources :::.._ " )
message ( STATUS "" )

include_directories ( AFTER ./ )

#TODO comment: must be before target linux
add_subdirectory ( ./provAndRep/)
add_subdirectory ( ./hal/ )
