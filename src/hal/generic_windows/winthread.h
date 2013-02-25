#ifndef _winthread_
#define _winthread_ 1

#include "hal/thread.h"


class WinThread : public Thread
{

public:

	WinThread ( Threadable * obj, ThreadConfig & cfg );

	~WinThread ()
	{
		KillThread();
	}

	void StartThread ();

	void StopThread ()
	{
		running = false;
	}

	void KillThread ();

private:

	static void *startHelper ( WinThread * obj );

	winthread_t ThreadID;

};



#endif // _winthread_
