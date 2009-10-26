#include "hal/syscall.h"

#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

int SysCall::_usleep ( unsigned long usec ) {
    return usleep( usec );
}

//safe on both 32 & 64 bit. 
unsigned long SysCall::_GetCurrentTimeInUSec () {
    struct timeval tp;
    gettimeofday ( &tp, NULL );
    unsigned long tm = tp.tv_sec * 1000000 + tp.tv_usec;
    return tm;
}

void SysCall::_exit ( int status ) {
    exit( status ); 
}
    
