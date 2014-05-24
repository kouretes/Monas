#ifndef SYSTEMTHREAD_HPP
#define SYSTEMTHREAD_HPP

#include "core/architecture/executable/IExecutable.hpp"
#include <boost/thread.hpp>


namespace KSystem
{

	class SystemThread : public IExecutable
	{

	public:

		SystemThread ( bool start = false ) : running(false)
		{
			if ( start )
				StartThread();
		}

		virtual ~SystemThread()
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
			bThread = boost::thread( &SystemThread::startHelper , this);
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



		virtual void startHelper ()
		{
			while (running)
				this->Execute();
		}

    private:
        boost::thread bThread;


	};

};

#endif // SYSTEMTHREAD_HPP
