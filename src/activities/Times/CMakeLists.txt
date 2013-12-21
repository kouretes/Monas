set ( ActivityName Times )

set ( ActivitySrcs  Times.cpp
)

include ( ../activTemplate.cmake )



set ( ActivityLibraries ${ActivityLibraries} Times ${SpecialActionLibraries} PARENT_SCOPE )
