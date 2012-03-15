
message ( STATUS "" )
message ( STATUS " _..::: Configuring TestBehavior :::.._ " )
message ( STATUS "" )

#add_library ( TestBehavior ${LibBuildType}  TestBehavior.cpp )  
              
add_library ( TestBehavior SHARED
        TestBehavior.cpp )
        
#target_link_libraries ( TestBehavior statechartEngine narukom )

install ( TARGETS TestBehavior
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} TestBehavior PARENT_SCOPE )
	