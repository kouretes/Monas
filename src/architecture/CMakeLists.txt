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



if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )

add_library ( MonadCore ${LibBuildType}   
                        IActivity.cpp
                        talws.cpp 
)

if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  target_link_libraries ( MonadCore dl )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )




target_link_libraries ( MonadCore M_tinyxml )

install ( TARGETS MonadCore 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )   

set ( TargetLinkLibraries ${TargetLinkLibraries} MonadCore narukomlib  PARENT_SCOPE ) 
#TODO add narukom link to cmake in narukom
   
