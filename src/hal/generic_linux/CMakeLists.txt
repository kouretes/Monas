message ( STATUS "" )
message ( STATUS " _..::: Configuring for Generic Linux Platform :::.._ " )
message ( STATUS "" )

# TODO  Cmake find pthreads

add_library( GenLinux ${LibBuildType} 
                      syscall.cpp 
                      dlib_fnc.cpp 
)

target_link_libraries( GenLinux pthread dl )

set ( TargetLinkLibraries ${TargetLinkLibraries} GenLinux PARENT_SCOPE )

install ( TARGETS GenLinux 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )
