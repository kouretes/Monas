message ( STATUS "" )
message ( STATUS " _..::: Configuring KMonitor :::.._ " )
message ( STATUS "" )

cmake_minimum_required( VERSION 2.6 )

PROJECT( KMonitor )

FIND_PACKAGE( Qt4 REQUIRED )

FIND_PACKAGE(Boost COMPONENTS date_time thread filesystem system REQUIRED)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${Boost_INCLUDE_DIRS} ../../src/)

ADD_SUBDIRECTORY( ../../external "${CMAKE_CURRENT_BINARY_DIR}/external")
ADD_SUBDIRECTORY( ../../src/architecture/narukom "${CMAKE_CURRENT_BINARY_DIR}/narukom")

SET( QT_USE_QTSVG TRUE )

SET( KMonitor_SOURCES	main.cpp
						KMonitor.cpp
						KGUIMessenger.cpp
						HostsComboBox.cpp
						KFieldScene.cpp
						GraphicalRobotElement.cpp
						LocalWorldController.cpp
						KMapScene.cpp
						KCamView.cpp
						GlobalWorldController.cpp
						KccHandler.cpp
						KccLabel.cpp
						SensorsController.cpp
						XMLHandler.cpp
						KSonarsController.cpp
						CommandsController.cpp
						../../src/architecture/XmlManager/XmlManager.cpp
						../../src/activities/Vision/KSegmentator.cpp
						../../src/hal/generic_linux/syscall.cpp
						../../src/messages/Network.pb.cc
						../../src/messages/VisionObservations.pb.cc
						../../src/messages/WorldInfo.pb.cc
						../../src/messages/Gamecontroller.pb.cc
						../../src/messages/motion.pb.cc
						../../src/messages/SensorsMessage.pb.cc
						../../src/messages/motion.pb.cc
						../../src/messages/ObstacleAvoidanceMessage.pb.cc
						../../src/messages/Kimage.pb.cc 
						../../src/messages/GUICommunication.pb.cc 
	)
SET( KMonitor_HEADERS	KMonitor.h
						KGUIMessenger.h
						HostsComboBox.h
						GraphicalRobotElement.h
						LocalWorldController.h
						KCamView.h
						GlobalWorldController.h
						KccHandler.h
						KccLabel.h
						SensorsController.h
						XMLHandler.h
						KSonarsController.h
						CommandsController.h
	) 
SET( KMonitor_FORMS KMonitor.ui
					KccHandler.ui
					XMLHandler.ui
					SensorsController.ui
					KSonarsController.ui
					KCamView.ui
					GlobalWorldController.ui
					LocalWorldController.ui
					CommandsController.ui
	)
SET( KMonitor_RESOURCES KMonitorResources.qrc )

QT4_WRAP_CPP( KMonitor_HEADERS_MOC ${KMonitor_HEADERS} )
QT4_WRAP_UI( KMonitor_FORMS_HEADERS ${KMonitor_FORMS} )
QT4_ADD_RESOURCES( KMonitor_RESOURCES_RCC ${KMonitor_RESOURCES} )

INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ADD_EXECUTABLE( KMonitor 	${KMonitor_SOURCES}
							${KMonitor_HEADERS_MOC}
							${KMonitor_FORMS_HEADERS}
							${KMonitor_RESOURCES_RCC}
	    	  )

TARGET_LINK_LIBRARIES( KMonitor ${QT_LIBRARIES} M_tinyxml narukom ${Boost_LIBRARIES})
