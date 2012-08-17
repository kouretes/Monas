
set ( SpecialActionName ALstandUpCross2011 )

set ( SpecialActionSrcs ALstandUpCross2011.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
