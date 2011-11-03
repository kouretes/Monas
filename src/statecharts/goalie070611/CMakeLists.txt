
message ( STATUS "" )
message ( STATUS " _..::: Configuring goalie070611 :::.._ " )
message ( STATUS "" )

add_library ( goalie070611 SHARED
        goalie070611.cpp )

#target_link_libraries ( goalie070611 statechartEngine narukom )

install ( TARGETS goalie070611
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} goalie070611 PARENT_SCOPE )

