message ( STATUS "" )
message ( STATUS " _..::: Configuring Elements :::.._ " )
message ( STATUS "" )


set(ELEMENT_SRCS
      buffer/LockedBuffer.hpp
      factory/Registrar.hpp
      factory/Factory.hpp
      math/Common.hpp
      math/KMat.hpp
      math/Specific.hpp
      customTypes/K3DArray.hpp
      customTypes/ReservaplePrioryQueue.hpp
      StringRegistry.hpp
      Singleton.hpp
      KStandard.hpp
      Logger.hpp
)

add_custom_target(Monas-Elements SOURCES ${ELEMENT_SRCS}  )



#add_subdirectory ( ./buffer/ )


