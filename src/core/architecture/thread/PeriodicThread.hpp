#ifndef PERIODICTHREAD_HPP
#define PERIODICTHREAD_HPP

#include "ThreadConfig.hpp"

#include "hal/SystemThread.hpp"
#include "hal/syscall.h"
#include "hal/smart_timer.h"

#include "core/include/Logger.hpp"



namespace KSystem
{
	class PeriodicThread : public SystemThread
	{

	public:


		PeriodicThread ( ThreadConfig &c, bool start = false ) : SystemThread(start)
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
