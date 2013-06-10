message ( STATUS "" )
message ( STATUS " _..::: Configuring KMonitor :::.._ " )
message ( STATUS "" )

cmake_minimum_required( VERSION 2.6 )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

PROJECT( KMonitor )

FIND_PACKAGE( Qt4 REQUIRED )

FIND_PACKAGE(Boost COMPONENTS date_time thread filesystem system REQUIRED)


SET( QT_USE_QTSVG TRUE )

SET( KMonitor_SOURCES	main.cpp
						KMonitor.cpp
						KGUIMessenger.cpp
						HostsComboBox.cpp
						KFieldScene.cpp
						GraphicalRobotElement.cpp
						LocalWorldController.cpp
						KMapScene.cpp
						GlobalWorldController.cpp
						KccHandler.cpp
						KccLabel.cpp
						SensorsController.cpp
						XMLHandler.cpp
						KSonarsController.cpp
						CommandsController.cpp
						../../core/architecture/configurator/Configurator.cpp
						../../core/architecture/configurator/ConfigNode.cpp
						../../activities/Vision/KSegmentator.cpp
						../../hal/generic_linux/syscall.cpp
	)
SET( KMonitor_HEADERS	KMonitor.h
						KGUIMessenger.h
						HostsComboBox.h
						GraphicalRobotElement.h
						LocalWorldController.h
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

ADD_EXECUTABLE( KMonitor EXCLUDE_FROM_ALL ${KMonitor_SOURCES}
							${KMonitor_HEADERS_MOC}
							${KMonitor_FORMS_HEADERS}
							${KMonitor_RESOURCES_RCC}
	    	  )

SET_TARGET_PROPERTIES( KMonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})

TARGET_LINK_LIBRARIES( KMonitor ${QT_LIBRARIES} M_tinyxml messaging MessageLib MessageSystemLib ${Boost_LIBRARIES})
