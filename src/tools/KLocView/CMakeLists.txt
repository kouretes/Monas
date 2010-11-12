message ( STATUS "" )
message ( STATUS " _..::: Configuring KLocView :::.._ " )
message ( STATUS "" )

PROJECT( KLocView )
#FIND_PACKAGE( OpenCV REQUIRED )


#add_executable ( stateTest statechartTest.cpp ../messages/planMsg.pb.cc )
#use_lib(KLocView LIBVISION OPENCV)
 find(OPENCV NO_MODULE QUIET)
#target_link_libraries ( stateTest statechartEngine narukom MonadCore GenLinux)


set(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR})
ADD_EXECUTABLE( KLocView KLocView.cpp  KfieldGui.cpp ../../../external/PracticalSocket/PracticalSocket.cpp
          ../../messages/VisionObservations.pb.cc ../../messages/WorldInfo.pb.cc )
TARGET_LINK_LIBRARIES( KLocView ${OPENCV_LIBRARIES} MessageLib )


install ( TARGETS KLocView
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   . #lib
            ARCHIVE DESTINATION   .)#lib/static )
