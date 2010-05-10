project( Kouretes2009 )

cmake_minimum_required( VERSION 2.6.4 )

message( STATUS "" )
message( STATUS " _..::: Configuring Architecture :::.._ " )
message( STATUS "" )

#set( CMAKE_VERBOSE_MAKEFILE on )

add_definitions ( -Wall )
add_definitions ( -m32 )

set ( TargetPlatform linux CACHE STRING 
        "select the targeted platform, available platforms are linux, windows, and robot" )


set ( RobotPlatform nao CACHE STRING
        "Select robotic platform. Available platforms are nao and aibo" )



add_subdirectory( external ) 
add_subdirectory( src )
