
set ( SpecialActionName ALstandUpBack2009 )

set ( SpecialActionSrcs ALstandUpBack2009.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

