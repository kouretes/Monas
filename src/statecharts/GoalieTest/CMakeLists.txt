
message ( STATUS "" )
message ( STATUS " _..::: Configuring GoalieTest :::.._ " )
message ( STATUS "" )

#add_library ( GoalieTest ${LibBuildType}  GoalieTest.cpp )  
              
add_library ( GoalieTest SHARED
        GoalieTest.cpp )
        
#target_link_libraries ( GoalieTest statechartEngine narukom )

install ( TARGETS GoalieTest
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GoalieTest PARENT_SCOPE )
	