#ifndef _syscall_h_
#define _syscall_h_  1

#include <string>

class SysCall {

    public:

        static int _usleep ( unsigned long usec ) ;

        static unsigned long _GetCurrentTimeInUSec ( ) ;

        static void _exit ( int status );

        static bool _GetCwd ( std::string& path );

};

#endif // _syscall_h_
