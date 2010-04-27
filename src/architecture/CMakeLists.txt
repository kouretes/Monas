message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./narukom/ )
add_subdirectory ( ./statechartEngine/ )

# TODO
include_directories ( AFTER narukom/
                            narukom/message_definitions/
                            narukom/messages/
                            narukom/network/
                            narukom/pub_sub/
                            narukom/system/ )

add_library ( ArchCore   IActivity.cpp
                         talws.cpp )

set ( TargetLinkLibraries ${TargetLinkLibraries} ArchCore PARENT_SCOPE )
   
