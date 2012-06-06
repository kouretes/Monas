add_executable(XmlConfigurator testXmlConfigurator.cpp XmlConfigurator.cpp XmlConfigurator.h)

target_link_libraries ( XmlConfigurator M_tinyxml)


install ( TARGETS  XmlConfigurator #stateTest
            RUNTIME DESTINATION   bin)
