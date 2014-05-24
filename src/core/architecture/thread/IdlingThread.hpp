#ifndef IDLINGTHREAD_HPP
#define IDLINGTHREAD_HPP

#include "hal/SystemCondVar.hpp"
#include "hal/SystemMutex.hpp"
#include "PeriodicThread.hpp"

namespace KSystem
{

	class IdlingThread : public PeriodicThread
	{

	public:

		IdlingThread ( ThreadConfig &c, bool start = false ) : PeriodicThread(c,start),idling(false)
		{
		}

		void setIdling(bool id){idling=id;};

		void StartThread()
		{
			running = true;
			PeriodicThread::StartThread();
		}

		void wakeUpThread()
		{
			KSystem::SystemMutex::scoped_lock cvlock(mutexCondSleeponit);
			running=true;
			condSleeponit.notify_one();
		}
		void idleThread()
		{
			KSystem::SystemMutex::scoped_lock cvlock(mutexCondSleeponit);
			running=false;
		}
	protected:
		void startHelper ()
		{
			while(idling)
			{
				PeriodicThread::startHelper();
				SystemMutex::scoped_lock cond_lock(mutexCondSleeponit);
				while(running==false)
					condSleeponit.wait(cond_lock);

			}
			if(!idling) //Regular run
				PeriodicThread::startHelper();



		}
		virtual const std::string GetName() const = 0;
		SystemCondVar condSleeponit;
		SystemMutex   mutexCondSleeponit;
		bool idling;


	};

};

#endif // _idlethread_h_
