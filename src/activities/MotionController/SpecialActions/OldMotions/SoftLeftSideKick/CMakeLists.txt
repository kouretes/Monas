
set ( SpecialActionName SoftLeftSideKick )

set ( SpecialActionSrcs SoftLeftSideKick.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
