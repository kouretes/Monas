
set ( SpecialActionName ALstandUpSit2011 )

set ( SpecialActionSrcs ALstandUpSit2011.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
