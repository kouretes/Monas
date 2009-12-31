message ( STATUS "" )
message ( STATUS " _..::: Configuring ${ProviderName} :::.._ " )
message ( STATUS "" )


set ( TargetLinkLibraries ${TargetLinkLibraries} ${ProviderName} PARENT_SCOPE )

add_library ( ${ProviderName} STATIC ${ProviderSrcs} )




#___________________________________________________________________
#__________________  Automatic Code Generetion _____________________
#___________________________________________________________________



#_____________________________________________________________________
# Check that the name of the Provider is the same with the header file 

file ( READ ${ProviderName}.h FileData )

if ( NOT FileData )
   message ( "Warning: ${CMAKE_CURRENT_SOURCE_DIR}/${ProviderName}.h does not exist!" )
   message ( "Warning: The ProviderName, the directory, the filename of the header file and the class name" )
   message ( "         must have the same name. The name IS case sensitive! " )
endif ( NOT FileData )


#_______________________________________________________________________________
# The read file has to declare a class with the same name as the Provider's Name 

string ( REGEX MATCH "class ${ProviderName} " ClassFound ${FileData} )

if ( "A${ClassFound}A" STREQUAL "AA" )
    message ( "Warning: Cound not find class ${ProviderName} in ${ProviderName}.h ")
endif ( "A${ClassFound}A" STREQUAL "AA" )


#_____________________________________________________________________________________
# Auto-generate the cpp code that needed to register the new provider into the factory

file ( APPEND ../providerRegistry.h "\n" )
file ( APPEND ../providerRegistry.h "#include \"${ProviderName}/${ProviderName}.h\" \n" )
file ( APPEND ../providerRegistry.h "namespace { \n" )
file ( APPEND ../providerRegistry.h "   ProviderRegistrar<${ProviderName}>::Type temp_${ProviderName}\(\"${ProviderName}\"\); \n")
file ( APPEND ../providerRegistry.h "}\n" )
file ( APPEND ../providerRegistry.h "\n" )


#___________________________________________________________________
#_________________  End of Automatic Code Generetion _______________
#___________________________________________________________________


