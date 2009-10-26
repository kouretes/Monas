

message ( "" )
message ( " _..::: Configuring Sources :::.._ " )
message ( "" )

include_directories ( AFTER ./ )

#TODO comment: must be before target linux
add_subdirectory ( ./provAndRep/)
add_subdirectory ( ./hal/ )
