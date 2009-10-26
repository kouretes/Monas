#ifndef _syscall_h_
#define _syscall_h_  1

#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

class SysCall {

    public:

        static int _usleep ( unsigned long usec ) ;

        static unsigned long _GetCurrentTimeInUSec ( ) ;

        static void _exit ( int status ); 

};

#endif // _syscall_h_
