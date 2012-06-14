message ( STATUS "" )
message ( STATUS " _..::: Configuring Special Actions :::.._ " )
message ( STATUS "" )

file ( GLOB SpInDir . * )

foreach ( spFile ${SpInDir} )
  if ( IS_DIRECTORY ${spFile} )
    set ( spSubDirs ${spSubDirs}  ${spFile}  )
    message ( STATUS "Special Action ${spFile} detected and added to the building tree" )
  endif ( IS_DIRECTORY ${spFile} )
endforeach ( spFile )


foreach ( spdir ${spSubDirs} )
   add_subdirectory( ${spdir} )
endforeach ( spdir ${spSubDirs} )

set ( SpecialActionLibraries ${SpecialActionLibraries}  PARENT_SCOPE)
