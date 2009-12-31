message ( STATUS "" )
message ( STATUS " _..::: Building Architecture for Linux Platform :::.._ " )
message ( STATUS "" )

add_executable ( kourarch main.cpp )

if ( "${ModuleBuildType}" STREQUAL "MODULE" )
  add_definitions ( -DDLIB_FUNCTIONALITY )
  target_link_libraries ( kourarch dl )
endif ( "${ModuleBuildType}" STREQUAL "MODULE" )

target_link_libraries( kourarch tinyxml ${TargetLinkLibraries} ${ModuleLibraries})

