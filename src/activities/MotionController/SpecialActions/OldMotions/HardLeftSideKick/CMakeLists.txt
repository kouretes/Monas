
set ( SpecialActionName HardLeftSideKick )

set ( SpecialActionSrcs HardLeftSideKick.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
