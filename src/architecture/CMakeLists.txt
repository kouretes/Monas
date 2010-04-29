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

    
if ( "${ModuleBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
  target_link_libraries ( ArchCore dl )
endif ( "${ModuleBuildType}" STREQUAL "MODULE" )

set ( TargetLinkLibraries ${TargetLinkLibraries} ArchCore narukomlib tinyxml PARENT_SCOPE ) 
#TODO add narukom link to cmake in narukom
   
