
message ( STATUS "" )
message ( STATUS " _..::: Configuring goalie :::.._ " )
message ( STATUS "" )

add_library ( goalie SHARED
        goalie.cpp )

target_link_libraries ( goalie)

install ( TARGETS goalie
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} goalie PARENT_SCOPE )

