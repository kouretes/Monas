message ( STATUS "" )
message ( STATUS " _..::: Configuring for Generic Windows Platform :::.._ " )
message ( STATUS "" )


add_library( GenWindows STATIC winthread.cpp syscall.cpp dlib_fnc.cpp)

set ( TargetLinkLibraries ${TargetLinkLibraries} GenWindows PARENT_SCOPE )

install ( TARGETS GenWindows 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )
