
set ( SpecialActionName LieDown )

set ( SpecialActionSrcs LieDown.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
