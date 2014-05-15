message ( STATUS "" )
message ( STATUS " _..::: Configuring Architecture :::.._ " )
message ( STATUS "" )

add_subdirectory ( ./messaging/ )
#add_subdirectory ( ./statechartEngine/ )

if ( "${ActivityBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
endif ( "${ActivityBuildType}" STREQUAL "MODULE" )

add_library ( MonasCore ${LibBuildType}
			executable/IExecutable.hpp
			executable/activity/IActivity.hpp
			executable/activity/IActivity.cpp
			executable/provider/IProvider.hpp
			executable/provider/IProvider.cpp
			thread/PeriodicThread.hpp
			thread/IdlingThread.hpp
			thread/ThreadConfig.hpp
			thread/agent/Agent.hpp
			thread/agent/Agent.cpp
			thread/agent/AgentTiming.hpp
			thread/agent/AgentConfig.hpp
			configurator/Configurator.hpp
			configurator/Configurator.cpp
			configurator/ConfigNode.hpp
			configurator/ConfigNode.cpp
			time/StopWatch.hpp
			time/Profiler.hpp
			time/SystemClockProvider.hpp
			time/SystemClockProvider.cpp
			time/IClock.hpp
			time/TimeTypes.hpp
			time/Timer.hpp
			talws.h
			talws.cpp
			statechartWrapper.h
)

target_link_libraries ( MonasCore M_tinyxml z )

install ( TARGETS MonasCore
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries  MonasCore messaging  ${TargetLinkLibraries} CACHE INTERNAL "" )

