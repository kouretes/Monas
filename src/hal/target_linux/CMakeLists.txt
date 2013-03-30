message ( STATUS "" )
message ( STATUS " _..::: Building Architecture for Linux Platform :::.._ " )
message ( STATUS "" )

set(MonasTargetSources ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp CACHE INTERNAL "")

function(Monas_Finalize)
	find_package(Boost COMPONENTS date_time thread system filesystem REQUIRED)
	find_package( ZLIB REQUIRED)
	message ( STATUS " _..::: Making Monas Executable Target :::.._ " )

	add_executable ( Monas ${MonasTargetSources} )

	target_link_libraries( Monas    -Wl,--no-as-needed -lrt  -Wl,--no-as-needed ${ActivityLibraries}  ${ProviderLibraries} -Wl,--as-needed ${TargetLinkLibraries} -Wl,--as-needed ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} )

	set_target_properties(Monas PROPERTIES BUILD_WITH_INSTALL_RPATH true)
	install ( TARGETS Monas
		    RUNTIME DESTINATION   bin
		    LIBRARY DESTINATION   lib
		    ARCHIVE DESTINATION   lib/static )
endfunction(Monas_Finalize)


