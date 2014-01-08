#ifndef THREAD_HPP
#define THREAD_HPP

#include "core/architecture/executable/IExecutable.hpp"
#include <boost/thread.hpp>






namespace KSystem
{

	class Thread : public IExecutable
	{

	public:

		Thread ( bool start = false ) : running(false)
		{
			if ( start )
				StartThread();
		}

		virtual ~Thread()
		{
			;
		}

		bool IsRunning() const
		{
			return running;
		}

		virtual void StartThread()
		{
			running = true;
			bThread = boost::thread( &Thread::startHelper , this);
		}

		virtual void StopThread()
		{
			running = false;
		}

		virtual void KillThread()
		{
			StopThread(); //TODO
		}

		void JoinThread()
		{
			bThread.join();
		}

	protected:
		volatile bool running;

		boost::thread bThread;

		virtual void startHelper ()
		{
			while (running)
				this->Execute();
		}




	};

};

#endif // _thread_h_
