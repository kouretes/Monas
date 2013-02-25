function(Monas_global_add_definitions ndef)
 
set( MonasDefinitions ${MonasDefinitions} ${ndef}  CACHE INTERNAL "" )

add_definitions(${MonasDefinitions})

endfunction(Monas_global_add_definitions)
