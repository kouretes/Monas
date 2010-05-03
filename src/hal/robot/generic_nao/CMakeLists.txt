message ( STATUS "" )
message ( STATUS " _..::: Configuring Generic Nao :::.._ " )
message ( STATUS "" )

add_library ( GenericNao  ${LibBuildType}
                          kAlBroker.cpp 
)

install ( TARGETS GenericNao 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries ${TargetLinkLibraries} GenericNao  PARENT_SCOPE ) 
