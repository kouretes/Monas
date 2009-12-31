message ( STATUS "" )
message ( STATUS " _..::: Building Architecture for Linux Platform :::.._ " )
message ( STATUS "" )


add_executable ( kourarch main.cpp )

target_link_libraries( kourarch tinyxml ${TargetLinkLibraries} )

