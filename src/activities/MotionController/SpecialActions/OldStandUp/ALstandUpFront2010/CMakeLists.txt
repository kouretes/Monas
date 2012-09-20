
set ( SpecialActionName ALstandUpFront2010 )

set ( SpecialActionSrcs ALstandUpFront2010.cpp 
)

include ( ../spActionTemplate.cmake )

qi_use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
