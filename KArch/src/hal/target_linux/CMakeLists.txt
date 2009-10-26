
message ( "" )
message ( " _..::: Building Architecture for Linux Platform :::.._ " )
message ( "" )


add_executable ( kourarch main.cpp )

target_link_libraries( kourarch tinyxml ${TargetLinkLibraries} )

