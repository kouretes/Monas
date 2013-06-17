#ifndef _periodicthread_h_
#define _periodicthread_h_ 1

#include "ThreadConfig.hpp"

#include "hal/Thread.hpp"
#include "hal/syscall.h"
#include "hal/smart_timer.h"

#include "core/include/Logger.hpp"



namespace KSystem
{
	class PeriodicThread : public Thread
	{

	public:


		PeriodicThread ( ThreadConfig &c, bool start = false ) : Thread(start)
		{
			IsRealTime = c.IsRealTime;
			Priority = c.Priority;
			ThreadPeriod = c.ThreadPeriod;
		}

		void StartThread()
		{
			running = true;
			bThread = boost::thread( &PeriodicThread::startHelper , this);
		}

	protected:
		virtual const std::string GetName() const = 0;
		void startHelper ()
		{
			while (running)
			{
				t.restart ();
				this->Execute();
				float el = t.elapsed();

				if ( el  > ThreadPeriod && ThreadPeriod > 0 )
					LogEntry(LogLevel::ExtraInfo,GetName())<< "Thread period cannot be achieved";
				else
					SysCall::_usleep( (ThreadPeriod - el) * 1000000L );
			}
		}
		KSystem::smart_timer t;

		bool IsRealTime;

		int Priority;

		float ThreadPeriod;


	};

};

#endif // _periodicthread_h_
