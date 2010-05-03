message ( STATUS "")
message ( STATUS  " _..::: TinyXml 2-5-3 :::.._ " )
message ( STATUS "")


add_definitions ( -DTINYXML_USE_STL )


add_library ( M_tinyxml ${ExternalSrcsBldType} 
        tinystr.cpp tinyxml.cpp 
        tinyxmlerror.cpp tinyxmlparser.cpp
)


install ( TARGETS M_tinyxml 
            RUNTIME DESTINATION   bin
            LIBRARY DESTINATION   lib
            ARCHIVE DESTINATION   lib/static )

