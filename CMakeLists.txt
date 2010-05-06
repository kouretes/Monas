project( Kouretes2009 )

cmake_minimum_required( VERSION 2.6.4 )

message( STATUS "" )
message( STATUS " _..::: Configuring Architecture :::.._ " )
message( STATUS "" )

#set( CMAKE_VERBOSE_MAKEFILE on )

add_definitions ( -Wall )
add_definitions ( -m32 )

add_subdirectory( external ) 
add_subdirectory( src )
