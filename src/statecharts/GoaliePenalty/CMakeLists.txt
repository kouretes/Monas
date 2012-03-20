
message ( STATUS "" )
message ( STATUS " _..::: Configuring GoaliePenalty :::.._ " )
message ( STATUS "" )

#add_library ( GoaliePenalty ${LibBuildType}  GoaliePenalty.cpp )  
              
add_library ( GoaliePenalty SHARED
        GoaliePenalty.cpp )
        
#target_link_libraries ( GoaliePenalty statechartEngine narukom )

install ( TARGETS GoaliePenalty
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} GoaliePenalty PARENT_SCOPE )
	