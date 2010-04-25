#ifndef _dlib_fnc_h_
#define _dlib_fnc_h_ 1

class DLibFnc {

    public:

        static void* _open ( const char * filename );

        static char* _error ( );

        static void* _sym ( void* handle, const char* symbol );

        static int _close ( void* handle );

};

#endif // _dlib_fnc_h_
