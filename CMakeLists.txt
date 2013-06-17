cmake_minimum_required( VERSION 2.6.4 )
project( Monas )
#Fancy cmake stuff to be put here:
add_subdirectory(cmake)

set ( MonasDefinitions "" CACHE INTERNAL "" )

#Clean linking
set ( TargetLinkLibraries  CACHE INTERNAL "" )


message( STATUS "" )
message( STATUS " _..::: Configuring Architecture :::.._ " )
message( STATUS "" )
 

include_directories ( AFTER src/ )
include_directories ( AFTER external/protobuf/src )

add_subdirectory( src/hal/) #Configure system
add_definitions (	-Wextra 
			-Wdouble-promotion
			-Wformat
			-Wunused
			-Wmissing-braces
			-Wmissing-include-dirs
			-Wparentheses
			-Wunused-parameter
			-Wfloat-equal)
message(STATUS "DEFINITIONS:" ${MonasDefinitions})
add_definitions (${MonasDefinitions})
add_subdirectory( src )
add_subdirectory( external )



install ( DIRECTORY ./config
          DESTINATION .
)
