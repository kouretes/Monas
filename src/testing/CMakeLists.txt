message ( STATUS "" )
message ( STATUS " _..::: Configuring Testing :::.._ " )
message ( STATUS "" )


option ( BuildTests "Compile testing code" )

if ( BuildTests )

    # TODO
    include_directories ( AFTER ../architecture/narukom/
                                ../architecture/narukom/message_definitions/
                                ../architecture/narukom/messages/
                                ../architecture/narukom/network/
                                ../architecture/narukom/pub_sub/
                                ../architecture/narukom/system/ )


    add_executable ( stateTest statechartTest.cpp ../messages/planMsg.pb.cc )

    target_link_libraries ( stateTest statechartEngine narukomlib ArchCore)

endif ( BuildTests )

