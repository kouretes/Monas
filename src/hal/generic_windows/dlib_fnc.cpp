#include "hal/dlib_fnc.h"

#include <dlfcn.h>

void* DLibFnc::_open ( const char * filename )
{
	return (void*) LoadLibrary( filename );
}

//char* DLibFnc::_error () {
//    return dlerror();
//}
//
//void* DLibFnc::_sym ( void* handle, const char* symbol ) {
//    return dlsym ( handle, symbol );
//}

int DLibFnc::_close ( void * handle )
{
	return FreeLibrary ( handle );
}

