
message ( STATUS "" )
message ( STATUS " _..::: Configuring KouretesAllConcurrentStatechart :::.._ " )
message ( STATUS "" )

add_library ( KouretesAllConcurrentStatechart ${LibBuildType}
        KouretesAllConcurrentStatechart.cpp )

target_link_libraries ( KouretesAllConcurrentStatechart statechartEngine narukom )

include( ../../hal/robot/nao-1.6.0/bootstrap.cmake )

use_lib(KouretesAllConcurrentStatechart ALCOMMON ALVALUE ALMATH LIBCORE TOOLS BOOST  )

install ( TARGETS KouretesAllConcurrentStatechart
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )

set ( StatechartLibraries ${StatechartLibraries} KouretesAllConcurrentStatechart PARENT_SCOPE )
