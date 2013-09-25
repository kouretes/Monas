message ( STATUS "" )
message ( STATUS " _..::: Configuring NAOKinematics Testing :::.._ " )
message ( STATUS "" )
if ( NOT ( ${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )


add_executable(Kinematics-testing Kinematics-testing.cpp )

target_link_libraries( Kinematics-testing GenericNao)
install ( TARGETS  Kinematics-testing 
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   lib
            ARCHIVE DESTINATION   lib/static )
