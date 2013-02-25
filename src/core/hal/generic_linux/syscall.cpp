#include "core/hal/syscall.h"

#include <unistd.h>
#include <sys/time.h>
#include <cstdlib>

int SysCall::_usleep ( unsigned long usec )
{
	return usleep( usec );
}

//safe on both 32 & 64 bit.
unsigned long SysCall::_GetCurrentTimeInUSec ()
{
	struct timeval tp;
	gettimeofday ( &tp, NULL );
	unsigned long tm = tp.tv_sec * 1000000 + tp.tv_usec;
	return tm;
}

void SysCall::_exit ( int status )
{
	exit( status );
}

bool SysCall::_GetCwd ( std::string& path )
{
	char _cPath[300];

	if ( ! getcwd ( _cPath, 300) )
		return false;

	path = _cPath;
	return true;
}


void SysCall::_Shutdown (  )
{
	//     execl("/sbin/shutdown","-h","now",0);
	system("shutdown -h now");
}


