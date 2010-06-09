message ( STATUS "" )
message ( STATUS " _..::: Configuring Testing :::.._ " )
message ( STATUS "" )


# TODO
include_directories ( AFTER ../architecture/narukom/
                            ../architecture/narukom/message_definitions/
                            ../architecture/narukom/messages/
                            ../architecture/narukom/network/
                            ../architecture/narukom/pub_sub/
                            ../architecture/narukom/system/ )


add_executable ( stateTest statechartTest.cpp ../messages/planMsg.pb.cc )

target_link_libraries ( stateTest statechartEngine narukom MonadCore GenLinux)

install ( TARGETS stateTest 
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   lib
            ARCHIVE DESTINATION   lib/static )
