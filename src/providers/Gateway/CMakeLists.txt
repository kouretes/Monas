set ( ProviderName Gateway)

set ( ProviderSrcs  Gateway.cpp Gateway.h )

include ( ../provTemplate.cmake )

if (RUN_ON_NAO)
	add_definitions(" -DRUN_ON_NAO ")
endif()

