project( Kouretes2009 )

cmake_minimum_required( VERSION 2.8 )

message( STATUS "" )
message( STATUS " _..::: Configuring Architecture :::.._ " )
message( STATUS "" )

#set( CMAKE_VERBOSE_MAKEFILE on )

set ( CMAKE_CXX_FLAGS -Wall )

add_subdirectory( external ) 
add_subdirectory( src )
