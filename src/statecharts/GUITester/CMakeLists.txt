
message ( STATUS "" )
message ( STATUS " _..::: Configuring GUITester :::.._ " )
message ( STATUS "" )

#add_library ( GUITester ${LibBuildType}  GUITester.cpp )  
              
add_library ( GUITester SHARED
        GUITester.cpp )
        
#target_link_libraries ( GUITester statechartEngine narukom )

install ( TARGETS GUITester
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GUITester PARENT_SCOPE )
	