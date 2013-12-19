message ( STATUS "" )
message ( STATUS " _..::: Configuring Testing :::.._ " )
message ( STATUS "" )


# TODO
#include_directories ( AFTER ../architecture/narukom/
#                            ../architecture/narukom/message_definitions/
#                            ../architecture/narukom/messages/
#                            ../architecture/narukom/network/
#                            ../architecture/narukom/pub_sub/
#                            ../architecture/narukom/system/ )

#add_executable ( stateTest statechartTest.cpp ../messages/planMsg.pb.cc )
#target_link_libraries ( stateTest statechartEngine narukom MonasCore GenLinux)

add_executable(KMat-testing KMat-testing.cpp )
add_executable(Kalman-testing kalmantest.cpp )
add_executable(Quaternion-testing Quaternion-testing.cpp )
add_subdirectory (Kinematics)

install ( TARGETS  KMat-testing Kalman-testing #stateTest
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   lib
            ARCHIVE DESTINATION   lib/static )
