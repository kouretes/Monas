#include "winthread.h"

#include "hal/syscall.h"

#include <process.h>

WinThread::WinThread ( Threadable * obj, ThreadConfig & cfg ) :
	Thread ( obj, cfg ), ThreadID(0)
{
	;
}


void WinThread::StartThread ()
{
	if (running)
		KillThread();

	running = true;
	ThreadID = (HANDLE) _beginthreadex(NULL, 0, (void) (*) ( void *) startHelper, this, 0, NULL);
}

void WinThread::KillThread ()
{
	StopThread();
	SysCall::_usleep ( 500000 );

	if ( ThreadID )
		;//::CloseHandle(m_thread);

	ThreadID = 0;
}


void *WinThread::startHelper ( WinThread * obj )
{
	unsigned long start, end;
	double ExecInterval, Freq2Time = (1 / (double)obj->config.ThreadFrequency) * 1000000;

	while ( obj->running )
	{
		start = SysCall::_GetCurrentTimeInUSec();
		obj->object->ThreadMain();
		end = SysCall::_GetCurrentTimeInUSec();
		ExecInterval = end - start;

		if ( ExecInterval > Freq2Time ) //&& ((++execution_num)%50)==0 ) //TODO choose number
			Logger::Instance()->WriteMsg("WinThread", "Decrease Freq!!!", Logger::ExtraInfo ); //TODO Find Name
		else
			SysCall::_usleep(Freq2Time - ExecInterval);
	}//TODO multiple

	_endthreadex(0);
	return 0;
}



