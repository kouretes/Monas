
message ( STATUS "" )
message ( STATUS " _..::: Configuring MyActivitySChart :::.._ " )
message ( STATUS "" )

add_library ( MyActivitySChart ${LibBuildType}
        MyActivitySChart.cpp )        

target_link_libraries ( MyActivitySChart statechartEngine narukom )

install ( TARGETS MyActivitySChart
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} MyActivitySChart PARENT_SCOPE )
	