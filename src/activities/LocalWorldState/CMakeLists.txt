include_directories( ../../../external/PracticalSocket)

set ( ActivityName LocalWorldState )

set ( ActivitySrcs  LocalWorldState.h LocalWorldState.cpp BallFilter.h BallFilter.cpp
newran/newran.h newran/newran.cpp newran/include.h newran/boolean.h newran/extreal.h newran/extreal.cpp newran/myexcept.cpp newran/myexcept.h
KLocalization.h KLocalization.cpp ../../../external/PracticalSocket/PracticalSocket.cpp
)

include ( ../activTemplate.cmake )

if(Webots)
  message ( STATUS " _..::: Configuring LocalWorldState for Webots :::.._ " )
  add_definitions(" -DWEBOTS ")
endif(Webots)
