message ( STATUS "" )
message ( STATUS " _..::: Configuring Sources :::.._ " )
message ( STATUS "" )


#Clean linking
set ( TargetLinkLibraries  CACHE INTERNAL "" )
include_directories ( AFTER ./ )


include_directories ( AFTER ../external/protobuf/src/ )
add_subdirectory ( ./messages/ )



set ( LibBuildType STATIC CACHE STRING
        "Choose build type for internal libraries. Available options are STATIC, SHARED and MODULE" )

if ( "x${LibBuildType}x" STREQUAL "xx" )
  message ( FATAL_ERROR "LibBuildType is not set!" )
elseif ( (NOT "${LibBuildType}" STREQUAL "STATIC") AND
         (NOT "${LibBuildType}" STREQUAL "SHARED") AND
         (NOT "${LibBuildType}" STREQUAL "MODULE") )
  message ( FATAL_ERROR "LibBuildType should be one of the following: STATIC, SHARED or MODULE" )
endif ( "x${LibBuildType}x" STREQUAL "xx" )

option ( Webots "Webots ..." )
set ( NaoQiVersion 1.10.10 CACHE STRING "Set the NaoQi version" )


add_subdirectory ( ./hal/ )
add_subdirectory ( ./architecture/ )
add_subdirectory ( ./activities/ )
add_subdirectory ( ./providers/ )
add_subdirectory ( ./statecharts/ )


#Call function that must be defined somewhere inside hal for proper linking

Monas_Finalize()


option ( BuildTests "Compile testing code" )




if ( BuildTests )
  add_subdirectory ( ./testing/ )
endif ( BuildTests )
