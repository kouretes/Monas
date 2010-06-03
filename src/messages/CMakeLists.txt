message ( STATUS "" )
message ( STATUS " _..::: Configuring Messages :::.._ " )
message ( STATUS "" )

file ( GLOB MessageLibSrcs . *.cc )

message ( STATUS "Messages detected:" )
foreach ( msg ${MessageLibSrcs} )
    message ( STATUS "Message ${msg} detected." )
endforeach ( msg )

add_library ( MessageLib ${LibBuildType}
                         ${MessageLibSrcs}   
)

target_link_libraries ( MessageLib narukom )
#TODO link with protocol buffer lib instead of narukomlib   

install ( TARGETS MessageLib 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )   

set ( TargetLinkLibraries ${TargetLinkLibraries} MessageLib PARENT_SCOPE ) 

