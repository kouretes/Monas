#ifndef _periodicthread_h_
#define _periodicthread_h_ 1

#include "core/hal/smart_timer.h"
#include "core/hal/thread.h"
#include "tools/logger.h"



namespace KSystem
{

	class ThreadConfig
	{

	public:

		bool IsRealTime;

		int Priority;

		float ThreadPeriod;

	};
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
				double el = t.elapsed();

				if ( el  > ThreadPeriod && ThreadPeriod > 0 )
					Logger::Instance().WriteMsg(GetName(), "Decrease Freq!!!", Logger::ExtraInfo );
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
