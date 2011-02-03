message ( STATUS "" )
message ( STATUS " _..::: Building Architecture for Linux Platform :::.._ " )
message ( STATUS "" )

# TODO
include_directories ( AFTER ../../architecture/narukom/
                            ../../architecture/narukom/message_definitions/
                            ../../architecture/narukom/messages/
                            ../../architecture/narukom/network/
                            ../../architecture/narukom/pub_sub/
                            ../../architecture/narukom/system/ )


add_executable ( Monas main.cpp )


target_link_libraries( Monas M_tinyxml ${TargetLinkLibraries} ${ActivityLibraries} )

install ( TARGETS Monas 
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   lib
            ARCHIVE DESTINATION   lib/static )


