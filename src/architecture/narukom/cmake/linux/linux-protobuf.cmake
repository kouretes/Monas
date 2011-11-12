find_package(PkgConfig)
pkg_check_modules(LIBProtobuf QUIET protobuf)
set(LIBProtobuf_DEFINITIONS ${LIBProtobuf_CFLAGS_OTHER})

message ( STATUS "Searching for protobuf libraries")


find_library(LIBProtobuf_LIBRARY protobuf
    HINTS ${LIBProtobuf_LIBDIR} ${LIBProtobuf_LIBRARY_DIRS} )

#set(LIBProtobuf_LIBRARIES ${LIBProtobuf_LIBRARY} )
#set(LIBProtobuf_INCLUDE_DIRS ${LIBProtobuf_INCLUDE_DIR} )


include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBProtobuf  DEFAULT_MSG
    LIBProtobuf_LIBRARY LIBProtobuf_INCLUDE_DIR)

mark_as_advanced(LIBProtobuf_INCLUDE_DIR LIBProtobuf_LIBRARY )


if( "A${LIBProtobuf_LIBRARY}A" STREQUAL "AA")
    set( ProtobufLibrary ${ConfigDir}/${TargetPlatform}/lib/libprotobuf.a)
else( "A${LIBProtobuf_LIBRARY}A" STREQUAL "AA")
    set( ProtobufLibrary ${LIBProtobuf_LIBRARY})
endif( "A${LIBProtobuf_LIBRARY}A" STREQUAL "AA")

message ( STATUS "Protobuf lib path: ${ProtobufLibrary}")
