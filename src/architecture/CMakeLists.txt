message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./narukom/ )

# TODO
include_directories ( AFTER narukom/
		    )


if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )

add_library ( MonasCore ${LibBuildType}
                        IActivity.cpp
                        talws.cpp
)

target_link_libraries ( MonasCore M_tinyxml )

install ( TARGETS MonasCore
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries ${TargetLinkLibraries} MonasCore narukom PARENT_SCOPE )
#TODO add narukom link to cmake in narukom

