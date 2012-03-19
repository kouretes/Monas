
set ( SpecialActionName PuntKick )

set ( SpecialActionSrcs PuntKick.cpp 
)

include ( ../spActionTemplate.cmake )

use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
