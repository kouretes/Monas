message ( STATUS "" )
message ( STATUS " _..::: Configuring Generic Nao :::.._ " )
message ( STATUS "" )
include( ../nao-${NaoQiVersion}/bootstrap.cmake ) 

add_library ( GenericNao  ${LibBuildType}
                          kAlBroker.cpp 
)
use_lib( GenericNao ALCOMMON)

install ( TARGETS GenericNao 
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries ${TargetLinkLibraries} GenericNao  PARENT_SCOPE ) 
