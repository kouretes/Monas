
message ( STATUS "" )
message ( STATUS " _..::: Configuring simplebehavior :::.._ " )
message ( STATUS "" )

add_library ( simplebehavior ${LibBuildType}
        simplebehavior.cpp )        

target_link_libraries ( simplebehavior statechartEngine narukom )

install ( TARGETS simplebehavior
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} simplebehavior PARENT_SCOPE )
	