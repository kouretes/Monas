#ifndef _idlethread_h_
#define _idlethread_h_ 1

#include "hal/CondVar.hpp"
#include "hal/Mutex.hpp"
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
			bThread = boost::thread( &IdlingThread::startHelper , this);
		}

		void wakeUpThread()
		{
			KSystem::Mutex::scoped_lock cvlock(mutexCondSleeponit);
			running=true;
			condSleeponit.notify_one();
		}
		void idleThread()
		{
			KSystem::Mutex::scoped_lock cvlock(mutexCondSleeponit);
			running=false;
		}
	protected:
		void startHelper ()
		{
			while(idling)
			{
				PeriodicThread::startHelper();
				Mutex::scoped_lock cond_lock(mutexCondSleeponit);
				while(running==false)
					condSleeponit.wait(cond_lock);

			}
			if(!idling) //Regular run
				PeriodicThread::startHelper();



		}
		virtual const std::string GetName() const = 0;
		CondVar condSleeponit;
		Mutex   mutexCondSleeponit;
		bool idling;


	};

};

#endif // _idlethread_h_
