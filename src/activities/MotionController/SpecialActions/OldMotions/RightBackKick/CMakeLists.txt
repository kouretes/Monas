
set ( SpecialActionName RightBackKick )

set ( SpecialActionSrcs RightBackKick.cpp 
)

include ( ../spActionTemplate.cmake )

use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )

 
