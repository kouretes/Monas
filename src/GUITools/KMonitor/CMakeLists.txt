message ( STATUS "" )
message ( STATUS " _..::: Configuring KMonitor :::.._ " )
message ( STATUS "" )

cmake_minimum_required( VERSION 2.6 )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

PROJECT( KMonitor )

FIND_PACKAGE(Boost COMPONENTS date_time thread filesystem system REQUIRED)

FIND_PACKAGE( Qt4 REQUIRED)

#FIND_PACKAGE(Qt5Core)

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
						KccCameraSettings.cpp
						../../core/architecture/configurator/Configurator.cpp
						../../core/architecture/configurator/ConfigNode.cpp
						../../robocup/vision/KSegmentator.cpp
						../../robocup/vision/KSegmentator.hpp
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
						KccCameraSettings.h
	) 
SET( KMonitor_FORMS KMonitor.ui
					KccHandler.ui
					XMLHandler.ui
					SensorsController.ui
					KSonarsController.ui
					GlobalWorldController.ui
					LocalWorldController.ui
					CommandsController.ui
					KccCameraSettings.ui
	)
SET( KMonitor_RESOURCES KMonitorResources.qrc )

#if( Qt4_FOUND )
	message (-=-=-= Using Qt4 =-=-=-)
	INCLUDE( ${QT_USE_FILE} )
	ADD_DEFINITIONS( ${QT_DEFINITIONS} )

	QT4_WRAP_CPP( KMonitor_HEADERS_MOC ${KMonitor_HEADERS} )
	QT4_WRAP_UI( KMonitor_FORMS_HEADERS ${KMonitor_FORMS} )
	QT4_ADD_RESOURCES( KMonitor_RESOURCES_RCC ${KMonitor_RESOURCES} )

	ADD_EXECUTABLE( KMonitor EXCLUDE_FROM_ALL ${KMonitor_SOURCES}
								${KMonitor_HEADERS_MOC}
								${KMonitor_FORMS_HEADERS}
								${KMonitor_RESOURCES_RCC}
	)
	SET_TARGET_PROPERTIES( KMonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})
	TARGET_LINK_LIBRARIES( KMonitor ${QT_LIBRARIES} M_tinyxml messaging MessageLib MessageSystemLib MonasCore ${Boost_LIBRARIES} )
#elseif(Qt5Core_FOUND)
#	message (-=-=-= Using Qt5 =-=-=-)
#	FIND_PACKAGE( Qt5Widgets REQUIRED )
#	FIND_PACKAGE( Qt5Gui REQUIRED )
#	FIND_PACKAGE( Qt5Svg REQUIRED )
#	add_definitions( -DQT_VERSION_5 )
#	QT5_WRAP_CPP( KMonitor_HEADERS_MOC ${KMonitor_HEADERS} )
#	QT5_WRAP_UI( KMonitor_FORMS_HEADERS ${KMonitor_FORMS} )
#	QT5_ADD_RESOURCES( KMonitor_RESOURCES_RCC ${KMonitor_RESOURCES} )
#
#	ADD_DEFINITIONS( ${Qt5Widgets_DEFINITONS})
#
#	ADD_EXECUTABLE( KMonitor EXCLUDE_FROM_ALL ${KMonitor_SOURCES}
#								${KMonitor_HEADERS_MOC}
#								${KMonitor_FORMS_HEADERS}
#								${KMonitor_RESOURCES_RCC} 
#				  )
#
#	SET_TARGET_PROPERTIES( KMonitor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX})
#	set_property(TARGET KMonitor APPEND PROPERTY INCLUDE_DIRECTORIES ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})
#
#	set(_flags ${Qt5Core_EXECUTABLE_COMPILE_FLAGS})
#	list(APPEND _flags ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS})
#	list(APPEND _flags ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS})
#	list(APPEND _flags ${Qt5Svg_EXECUTABLE_COMPILE_FLAGS})
#	list(REMOVE_DUPLICATES _flags)
#	set_target_properties(KMonitor PROPERTIES COMPILE_FLAGS ${_flags})
#
#	TARGET_LINK_LIBRARIES( KMonitor ${QT_LIBRARIES} M_tinyxml messaging MessageLib MessageSystemLib ${Boost_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Svg_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} /lib/i386-linux-gnu/libz.so.1)
#else(Qt4_FOUND)
#	message ( FATAL_ERROR "Qt4 or Qt5 required for this project" )
#endif(Qt4_FOUND)

