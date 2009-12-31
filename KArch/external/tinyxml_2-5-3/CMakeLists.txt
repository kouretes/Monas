message ( STATUS "")
message ( STATUS  " _..::: TinyXml 2-5-3 :::.._ " )
message ( STATUS "")

add_definitions ( -DTINYXML_USE_STL )

add_library ( tinyxml  STATIC
        tinystr.cpp tinyxml.cpp 
        tinyxmlerror.cpp tinyxmlparser.cpp
        )

