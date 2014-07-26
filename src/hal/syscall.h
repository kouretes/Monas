#ifndef _syscall_h_
#define _syscall_h_  1

#include <string>

class SysCall
{

public:

	static int _usleep ( unsigned long usec ) ;

	static void _exit ( int status );

	static void _Shutdown ( );

};

#endif // _syscall_h_
