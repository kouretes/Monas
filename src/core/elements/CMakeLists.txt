message ( STATUS "" )
message ( STATUS " _..::: Configuring Elements :::.._ " )
message ( STATUS "" )


set(ELEMENT_SRCS
      buffer/LockedBuffer.hpp
      StringRegistry.hpp
)

add_custom_target(Monas-Elements SOURCES ${ELEMENT_SRCS}  )



#add_subdirectory ( ./buffer/ )


