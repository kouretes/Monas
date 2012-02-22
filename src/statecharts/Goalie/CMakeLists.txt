
message ( STATUS "" )
message ( STATUS " _..::: Configuring Goalie :::.._ " )
message ( STATUS "" )

#add_library ( Goalie ${LibBuildType}  Goalie.cpp )  
              
add_library ( Goalie SHARED
        Goalie.cpp )
        
#target_link_libraries ( Goalie statechartEngine narukom )

install ( TARGETS Goalie
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} Goalie PARENT_SCOPE )
	