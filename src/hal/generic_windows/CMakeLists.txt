message ( STATUS "" )
message ( STATUS " _..::: Configuring for Generic Windows Platform :::.._ " )
message ( STATUS "" )


add_library( GenWindows ${LibBuildType}
                        winthread.cpp
                        syscall.cpp
                        dlib_fnc.cpp
)

set ( TargetLinkLibraries ${TargetLinkLibraries} GenWindows CACHE INTERNAL "" )

install ( TARGETS GenWindows
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )
