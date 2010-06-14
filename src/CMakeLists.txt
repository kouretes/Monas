message ( STATUS "" )
message ( STATUS " _..::: Configuring Sources :::.._ " )
message ( STATUS "" )

include_directories ( AFTER ./ )

include_directories ( AFTER ../external/protobuf/src/ )
#FIXME

set ( LibBuildType SHARED CACHE STRING 
        "Choose build type for internal libraries. Available options are STATIC, SHARED and MODULE" )

if ( "x${LibBuildType}x" STREQUAL "xx" )
  message ( FATAL_ERROR "LibBuildType is not set!" )
elseif ( (NOT "${LibBuildType}" STREQUAL "STATIC") AND
         (NOT "${LibBuildType}" STREQUAL "SHARED") AND
         (NOT "${LibBuildType}" STREQUAL "MODULE") )
  message ( FATAL_ERROR "LibBuildType should be one of the following: STATIC, SHARED or MODULE" )
endif ( "x${LibBuildType}x" STREQUAL "xx" )



add_subdirectory ( ./messages/ )
add_subdirectory ( ./architecture/ )

#TODO comment: must be before target linux
add_subdirectory ( ./activities/ )
add_subdirectory ( ./statecharts/ )
add_subdirectory ( ./hal/ )

option ( BuildTests "Compile testing code" )

if ( BuildTests )
  add_subdirectory ( ./testing/ )
endif ( BuildTests )
