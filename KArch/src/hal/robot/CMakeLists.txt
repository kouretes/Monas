message ( "" )
message ( " _..::: Configuring Robot Abstraction Layer :::.._ " )
message ( "" )



option ( ROBOT_NAO "set option to on if applicable" ON )

if ( ROBOT_NAO )
    add_subdirectory ( generic_nao )
    add_subdirectory ( nao_1_3_17 )
endif ( ROBOT_NAO )



option ( ROBOT_AIBO "set option to on if applicable" OFF )

if ( ROBOT_AIBO )
    add_subdirectory ( aibo )
endif ( ROBOT_AIBO )

