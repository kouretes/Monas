
message ( STATUS "" )
message ( STATUS " _..::: Configuring centerfor :::.._ " )
message ( STATUS "" )

add_library ( centerfor ${LibBuildType}
        centerfor.cpp )        

target_link_libraries ( centerfor statechartEngine narukom )

install ( TARGETS centerfor
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} centerfor PARENT_SCOPE )
	