##############################
#
# WELCOME
#
# This is the main cmake configuration file
# associated with your project
# You can define specific compilation options here
#
# More about cmake : www.cmake.org
#
# Using cmake command line :
# 1. Create a build directory : mkdir build
# 2. Configure cmake project : cd build; cmake ..
#   You can also generate project with your favorite IDE :
#   Example : For Mac osx use "cmake .. -G XCode"
#   see cmake help for more information
# 3. Edit project option : ccmake .
# 4. Build it : make
message ( STATUS "" )
message ( STATUS " _..::: Configuring KRobot :::.._ " )
message ( STATUS "" )


SET ( KROBOT_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/krobotmain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mainmodule.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mainmodule.cpp
)

set(KRobotTargetSources ${KROBOT_SRCS} CACHE INTERNAL "")

#option(KROBOT_IS_REMOTE
#  "is KROBOT remote?"
#  OFF
#)

if (KROBOT_IS_REMOTE)
 	add_definitions(" -DKROBOT_IS_REMOTE ")
endif()

function(Monas_Finalize)
      
	message ( STATUS " _..::: Making Krobot Target :::.._ " )
	#create_module( krobot )
	
	if (KROBOT_IS_REMOTE)
 		qi_create_bin(krobot ${KRobotTargetSources})
	else()
		qi_create_lib(krobot SHARED ${KRobotTargetSources})
	endif()
	qi_use_lib( krobot ALCOMMON ALMATH LIBCORE ALTOOLS ALVALUE BOOST )

	target_link_libraries ( krobot ${ActivityLibraries}  ${ProviderLibraries} ${TargetLinkLibraries}  M_tinyxml)


	message("Install:" ${CMAKE_INSTALL_PREFIX})
	install ( TARGETS krobot
			RUNTIME DESTINATION  bin
			LIBRARY DESTINATION  lib/naoqi
			ARCHIVE DESTINATION  lib/static )

endfunction(Monas_Finalize)






