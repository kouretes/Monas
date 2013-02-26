message ( STATUS "" )
message ( STATUS " _..::: Configuring Messages/System :::.._ " )
message ( STATUS "" )

file ( GLOB MessageSystemLibSrcs . *.cc )

message ( STATUS "Messages/System detected:" )
foreach ( msg ${MessageSystemLibSrcs} )
    message ( STATUS "Message ${msg} detected." )
endforeach ( msg )

add_library ( MessageSystemLib ${LibBuildType}
                         ${MessageSystemLibSrcs}
)

target_link_libraries ( MessageSystemLib messaging )
#TODO link with protocol buffer lib instead of narukomlib

install ( TARGETS MessageSystemLib
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries ${TargetLinkLibraries} MessageSystemLib CACHE INTERNAL "" )

