
message ( STATUS "" )
message ( STATUS " _..::: Configuring GoalieYellow :::.._ " )
message ( STATUS "" )

#add_library ( GoalieYellow ${LibBuildType}  GoalieYellow.cpp )  
              
add_library ( GoalieYellow SHARED
        GoalieYellow.cpp )
        
#target_link_libraries ( GoalieYellow statechartEngine narukom )

install ( TARGETS GoalieYellow
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GoalieYellow PARENT_SCOPE )
	