
set ( SpecialActionName ALstandUpFront2011 )

set ( SpecialActionSrcs ALstandUpFront2011.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
