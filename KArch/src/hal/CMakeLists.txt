message ( "" )
message ( " _..::: Configuring Hardware Abstraction Layer :::.._ " )
message ( "" )



option ( GENERIC_LINUX "set option to on if applicable" ON )

if ( GENERIC_LINUX )
    add_subdirectory ( generic_linux )
endif ( GENERIC_LINUX )




option ( TARGET_LINUX "set option to on if applicable" OFF )

if ( TARGET_LINUX )
    add_subdirectory ( target_linux )
endif ( TARGET_LINUX )


