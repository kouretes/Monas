message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )


add_subdirectory ( ./narukom/ )
add_subdirectory ( ./statechartEngine/ )
add_subdirectory ( ./robocup/)

# TODO

if (RUN_ON_NAO)
	add_definitions(" -DRUN_ON_NAO ")
endif()

if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )

add_library ( MonasCore ${LibBuildType}
			executables/IActivity.h
			executables/IActivity.cpp
			executables/IProvider.h
			executables/IProvider.cpp
			executables/agent.h
			executables/agent.cpp
			executables/agentTiming.h
			XmlManager/XmlManager.h
			XmlManager/XmlManager.cpp
			IExecutable.h
			talws.h
			talws.cpp
			config.h			
			statechartWrapper.h
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

