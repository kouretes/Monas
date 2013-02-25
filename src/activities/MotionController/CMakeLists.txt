if ( NOT (${TargetRobot} STREQUAL "nao" ) )
    return()
endif ( NOT (${TargetRobot} STREQUAL "nao" ) )

set ( ActivityName MotionController )

set ( ActivitySrcs  MotionController.cpp
                    KmeManager.cpp
                    KmeAction.cpp
                    XarManager.cpp
                    XarAction.cpp
                    KmexManager.cpp
                    KmexAction.cpp
)

include ( ../activTemplate.cmake )

qi_use_lib(${ActivityName} ALCOMMON ALVALUE ALMATH LIBCORE TOOLS)
add_subdirectory ( SpecialActions )


set ( ActivityLibraries ${ActivityLibraries} MotionController ${SpecialActionLibraries} PARENT_SCOPE )
