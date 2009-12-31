message ( STATUS "" )
message ( STATUS " _..::: Configuring for Generic Linux Platform :::.._ " )
message ( STATUS "" )

# TODO  Cmake find pthreads

add_library( GenLinux STATIC pthread.cpp syscall.cpp )

target_link_libraries( GenLinux  pthread )

set ( TargetLinkLibraries ${TargetLinkLibraries} GenLinux PARENT_SCOPE )

