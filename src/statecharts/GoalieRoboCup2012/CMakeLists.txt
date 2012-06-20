
message ( STATUS "" )
message ( STATUS " _..::: Configuring GoalieRoboCup2012 :::.._ " )
message ( STATUS "" )

#add_library ( GoalieRoboCup2012 ${LibBuildType}  GoalieRoboCup2012.cpp )  
              
add_library ( GoalieRoboCup2012 SHARED
        GoalieRoboCup2012.cpp )
        
#target_link_libraries ( GoalieRoboCup2012 statechartEngine narukom )

install ( TARGETS GoalieRoboCup2012
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GoalieRoboCup2012 PARENT_SCOPE )
	