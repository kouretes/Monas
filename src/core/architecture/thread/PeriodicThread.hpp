#ifndef PERIODICTHREAD_HPP
#define PERIODICTHREAD_HPP

#include "ThreadConfig.hpp"

#include "hal/SystemThread.hpp"
#include "hal/syscall.h"
#include "core/architecture/time/SystemClockProvider.hpp"
#include "core/architecture/time/Timer.hpp"

#include "core/include/Logger.hpp"



namespace KSystem
{
	class PeriodicThread : public SystemThread
	{

	public:


		PeriodicThread ( ThreadConfig &c, bool start = false ) :
		SystemThread(start),
		t(Time::SystemClockProvider::getSystemClock())
		{
			IsRealTime = c.IsRealTime;
			Priority = c.Priority;
			ThreadPeriod = KSystem::Time::ClockValue::fromFloat(c.ThreadPeriod);
		}
		void StartThread()
		{
			running = true;
			overhead=KSystem::Time::ClockValue::seconds(0);
            SystemThread::setThreadName(GetName());
			SystemThread::StartThread();
		}

	protected:
		virtual const std::string GetName() const = 0;
		void startHelper ()
		{
			while (running)
			{
			    t.restart ();
              	this->Execute();
				KSystem::Time::ClockValue el = t.duration();

				if ( el  > ThreadPeriod && ThreadPeriod > KSystem::Time::ClockValue::seconds(0) )
					LogEntry(LogLevel::ExtraInfo,GetName())<< "Thread period cannot be achieved";
				else
				{
                    t.restart();
                    float sleepfor=(ThreadPeriod-el-overhead).toFloat();
				    SysCall::_usleep( sleepfor * 1000000L );
				    overhead=((t.duration()-(ThreadPeriod - el-overhead) ));//+overhead)/2;
                }
                //LogEntry(LogLevel::ExtraInfo,GetName())<<overhead;


			}
		}
		bool timerfirst;
		KSystem::Time::Timer t;
		KSystem::Time::ClockValue overhead;

		bool IsRealTime;

		int Priority;

		KSystem::Time::ClockValue ThreadPeriod;




	};

};

#endif // _periodicthread_h_
