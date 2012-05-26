message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./narukom/ )
add_subdirectory ( ./statechartEngine/ )
add_subdirectory (./robocup/)

# TODO


if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )

add_library ( MonasCore ${LibBuildType}
                        IActivity.h
                        IActivity.cpp
                        talws.h
                        talws.cpp
			config.h
			archConfig.h
			agentConfig.h
)

target_link_libraries ( MonasCore M_tinyxml )

install ( TARGETS MonasCore
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries  MonasCore narukom  ${TargetLinkLibraries} CACHE INTERNAL "" )
#TODO add narukom link to cmake in narukom

