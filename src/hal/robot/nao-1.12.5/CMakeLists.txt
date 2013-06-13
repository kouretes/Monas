message ( STATUS "" )
message ( STATUS " _..::: Configuring NaoQi ver 1.12.5 :::.._ " )
message ( STATUS "" )


add_subdirectory ( krobot )

add_definitions (${MonasDefinitions}) #Work around: Monas definitions are added at root level. Meanwhile, add them here as well
add_library ( NaoQiDep  ${LibBuildType}
                        earLed.cpp
                        HeadAngleSensor.cpp
                        KImageExtractor.cpp
			NaoCamera.cpp
)

qi_use_lib( NaoQiDep ALCOMMON BOOST)

set ( TargetLinkLibraries ${TargetLinkLibraries} NaoQiDep)

install ( TARGETS NaoQiDep
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

