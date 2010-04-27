message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./narukom/ )
add_subdirectory ( ./statechartEngine/ )

set ( TargetLinkLibraries ${TargetLinkLibraries} PARENT_SCOPE )
