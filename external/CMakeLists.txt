message ( STATUS "" )
message ( STATUS " _..::: Configuring External Sources :::.._ " )
message ( STATUS "" )


set ( ExternalSrcsBldType STATIC CACHE STRING
        "External sources build type. Available options: STATIC and SHARED" )

if ( "x${ExternalSrcsBldType}x" STREQUAL "xx" )
    message ( FATAL_ERROR "ExternalSrcsBldType is not set!" )
elseif ( (NOT "${ExternalSrcsBldType}" STREQUAL "STATIC" ) AND
         (NOT "${ExternalSrcsBldType}" STREQUAL "SHARED") )
    message ( FATAL_ERROR "ExternalSrcsBldType must be STATIC or SHARED" )
endif ( "x${ExternalSrcsBldType}x" STREQUAL "xx" )



add_subdirectory ( ./tinyxml_2-5-3/ )
