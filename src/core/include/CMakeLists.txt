message ( STATUS "" )
message ( STATUS " _..::: Configuring Monas Include :::.._ " )
message ( STATUS "" )


set(INCLUDE_SRCS
      Blackboard.hpp
      IActivity.hpp
      IProvider.hpp
      MessageHub.hpp
      Configurator.hpp
      Logger.hpp
)

add_custom_target(Monas-Include SOURCES ${INCLUDE_SRCS}  )



#add_subdirectory ( ./buffer/ )


