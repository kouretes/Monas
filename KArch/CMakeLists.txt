project( Kouretes2009 )

cmake_minimum_required( VERSION 2.6 )

message( STATUS "" )
message( STATUS " _..::: Configuring Architecture :::.._ " )
message( STATUS "" )

#set( cmake_verbose_makefile on )

option ( MAKE_DEBUG "Set to on to enable debugging symbols" OFF )


if ( MAKE_DEBUG )
    add_definitions( -DDEBUG -g )
else ( MAKE_DEBUG )
    add_definitions( -Wall -O3 )
endif ( MAKE_DEBUG )

add_subdirectory( external ) 
add_subdirectory( src )
