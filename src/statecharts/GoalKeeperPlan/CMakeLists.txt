
message ( STATUS "" )
message ( STATUS " _..::: Configuring GoalKeeperPlan :::.._ " )
message ( STATUS "" )

add_library ( GoalKeeperPlan ${LibBuildType}
        GoalKeeperPlan.cpp )        

target_link_libraries ( GoalKeeperPlan statechartEngine narukom )

install ( TARGETS GoalKeeperPlan
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GoalKeeperPlan PARENT_SCOPE )
	