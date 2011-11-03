message ( STATUS "" )
message ( STATUS " _..::: Building Architecture for Linux Platform :::.._ " )
message ( STATUS "" )


set(MonasTargetSources ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp CACHE INTERNAL "")

function(Monas_Finalize)
	find_package(Boost COMPONENTS date_time thread system)

	message ( STATUS " _..::: Making Monas Executable Target :::.._ " )
	add_executable ( Monas ${MonasTargetSources} )
	target_link_libraries( Monas   ${ActivityLibraries}  ${TargetLinkLibraries}  ${Boost_LIBRARIES} )


	install ( TARGETS Monas
		    RUNTIME DESTINATION   bin
		    LIBRARY DESTINATION   lib
		    ARCHIVE DESTINATION   lib/static )
endfunction(Monas_Finalize)


