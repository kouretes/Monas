message ( STATUS "" )
message ( STATUS " _..::: Configuring NaoQi ver 1.12.5 :::.._ " )
message ( STATUS "" )

add_library ( NaoQiDep  ${LibBuildType}
                        earLed.cpp
                        HeadAngleSensor.cpp
                        KImageExtractor.cpp
)

qi_use_lib( NaoQiDep ALCOMMON BOOST)

set ( TargetLinkLibraries ${TargetLinkLibraries} NaoQiDep)

install ( TARGETS NaoQiDep
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

add_subdirectory ( krobot )
