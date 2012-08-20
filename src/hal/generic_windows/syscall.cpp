#include "hal/syscall.h"

#include <stdlib.h>
#include <Windows.h>

int SysCall::_usleep ( unsigned long usec )
{
	Sleep( usec / 1000 );
	return 0;
}

//TODO
//unsigned long SysCall::_GetCurrentTimeInUSec () {
//    struct timeval tp;
//    gettimeofday ( &tp, NULL );
//    unsigned long tm = tp.tv_sec * 1000000 + tp.tv_usec;
//    return tm;
//}

void SysCall::_exit ( int status )
{
	exit( status );
}

