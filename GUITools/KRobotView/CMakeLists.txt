message ( STATUS "" )
message ( STATUS " _..::: Configuring KRobotView :::.._ " )
message ( STATUS "" )
cmake_minimum_required(VERSION 2.6)

PROJECT( KRobotView )
FIND_PACKAGE( OpenCV REQUIRED )
FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(Protobuf)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

#add_executable ( stateTest statechartTest.cpp ../messages/planMsg.pb.cc )
#use_lib(KLocView LIBVISION OPENCV)
#find(OPENCV NO_MODULE QUIET)
#target_link_libraries ( stateTest statechartEngine narukom MonasCore GenLinux)


set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
message (STATUS ${CMAKE_BINARY_DIR})
# ${CMAKE_BINARY_DIR}
ADD_EXECUTABLE( KRobotView KRobotView.cpp  ../../external/PracticalSocket/PracticalSocket.cpp
          ../../src/messages/VisionObservations.pb.cc ../../src/messages/Kimage.pb.cc  ../../src/messages/WorldInfo.pb.cc )
TARGET_LINK_LIBRARIES( KRobotView ${OpenCV_LIBS} ${QT_LIBRARIES} ${PROTOBUF_LIBRARY} )


