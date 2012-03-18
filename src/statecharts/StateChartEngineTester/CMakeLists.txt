
message ( STATUS "" )
message ( STATUS " _..::: Configuring StateChartEngineTester :::.._ " )
message ( STATUS "" )

#add_library ( StateChartEngineTester ${LibBuildType}  StateChartEngineTester.cpp )  
              
add_library ( StateChartEngineTester SHARED
        StateChartEngineTester.cpp )
        
#target_link_libraries ( StateChartEngineTester statechartEngine narukom )

install ( TARGETS StateChartEngineTester
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} StateChartEngineTester PARENT_SCOPE )
	