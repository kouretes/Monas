#include "hal/syscall.h"

#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

int SysCall::_usleep ( unsigned long usec )
{
	return usleep( usec );
}


void SysCall::_exit ( int status )
{
	exit( status );
}



void SysCall::_Shutdown (  )
{
	//     execl("/sbin/shutdown","-h","now",0);
	system("shutdown -h now");
}


