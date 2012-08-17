
set ( SpecialActionName LeftBackKick )

set ( SpecialActionSrcs LeftBackKick.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
