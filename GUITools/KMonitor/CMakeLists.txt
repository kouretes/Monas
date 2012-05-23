message ( STATUS "" )
message ( STATUS " _..::: Configuring KMonitor :::.._ " )
message ( STATUS "" )

cmake_minimum_required( VERSION 2.6 )

PROJECT( KMonitor )

FIND_PACKAGE( Qt4 REQUIRED )
FIND_PACKAGE( OpenCV REQUIRED )

# necessary ? 
FIND_PACKAGE(Boost COMPONENTS date_time thread system)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS} ../../src/)

ADD_SUBDIRECTORY( ../../external "${CMAKE_CURRENT_BINARY_DIR}/external")
ADD_SUBDIRECTORY( ../../src/architecture/narukom "${CMAKE_CURRENT_BINARY_DIR}/narukom")

SET( QT_USE_QTSVG TRUE )

SET( KMonitor_SOURCES main.cpp KMonitor.cpp KGUIMessenger.cpp GWRemoteHosts.cpp LWRemoteHosts.cpp 
					KGraphicsView.cpp KFieldScene.cpp GraphicalRobotElement.cpp LWElementList.cpp
					KLabel.cpp KRobotMap.cpp LMElementList.cpp KMapView.cpp KMapScene.cpp
					../../src/hal/generic_linux/syscall.cpp
					../../src/messages/Network.pb.cc
					../../src/messages/VisionObservations.pb.cc
					../../src/messages/WorldInfo.pb.cc
					../../src/messages/Gamecontroller.pb.cc
					../../src/messages/motion.pb.cc
					../../src/messages/SensorsMessage.pb.cc
					../../src/messages/motion.pb.cc
					../../src/messages/ObstacleAvoidanceMessage.pb.cc
					  )
SET( KMonitor_HEADERS KMonitor.h KGUIMessenger.h GWRemoteHosts.h LWRemoteHosts.h KGraphicsView.h 
				 	GraphicalRobotElement.h LWElementList.h KLabel.h LMElementList.h KMapView.h) 
SET( KMonitor_FORMS KMonitor.ui )
SET( KMonitor_RESOURCES GuiResources.qrc )

QT4_WRAP_CPP( KMonitor_HEADERS_MOC ${KMonitor_HEADERS} )
QT4_WRAP_UI( KMonitor_FORMS_HEADERS ${KMonitor_FORMS} )
QT4_ADD_RESOURCES( KMonitor_RESOURCES_RCC ${KMonitor_RESOURCES} )

INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ADD_EXECUTABLE( KMonitor ${KMonitor_SOURCES}
		${KMonitor_HEADERS_MOC}
		${KMonitor_FORMS_HEADERS}
		${KMonitor_RESOURCES_RCC}
	      )

TARGET_LINK_LIBRARIES( KMonitor ${QT_LIBRARIES} ${OpenCV_LIBS} M_tinyxml narukom ${Boost_LIBRARIES} )
