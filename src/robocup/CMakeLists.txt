message ( STATUS "" )
message ( STATUS " _..::: Configuring RoboCup :::.._ " )
message ( STATUS "" )

add_library ( RoboCup  ${LibBuildType}
			CooperationHelper.cpp
			CooperationHelper.hpp
			vision/KSegmentator.cpp
			vision/KSegmentator.hpp

)

install ( TARGETS RoboCup
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )


set ( TargetLinkLibraries ${TargetLinkLibraries} RoboCup  CACHE INTERNAL "" )
