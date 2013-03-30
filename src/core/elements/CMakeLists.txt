message ( STATUS "" )
message ( STATUS " _..::: Configuring Elements :::.._ " )
message ( STATUS "" )


set(ELEMENT_SRCS
      buffer/LockedBuffer.hpp
      factory/Registrar.hpp
      factory/Factory.hpp
      math/Common.hpp
      math/KMat.hpp
      StringRegistry.hpp
      Singleton.hpp
)

add_custom_target(Monas-Elements SOURCES ${ELEMENT_SRCS}  )



#add_subdirectory ( ./buffer/ )


