
set ( SpecialActionName ALstandUpBack2011_2 )

set ( SpecialActionSrcs ALstandUpBack2011_2.cpp
)

include ( ../spActionTemplate.cmake )

use_lib(${SpecialActionName}  LIBCORE TOOLS LIBCORE ALCOMMON)

target_link_libraries ( ${SpecialActionName} NaoQiDep )


