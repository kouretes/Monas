#ifndef SYSTEMTHREAD_HPP
#define SYSTEMTHREAD_HPP

#include "core/architecture/executable/IExecutable.hpp"
#include <boost/thread.hpp>


namespace KSystem
{

	class SystemThread : public IExecutable
	{

	public:

		SystemThread ( bool start = false ) : running(false), hasname(false)
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
			if(hasname) enforceName();
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
        void setThreadName(std::string newname){ hasname=true; name=newname;};
	protected:


		volatile bool running;



		virtual void startHelper ()
		{
			while (running)
				this->Execute();
		}

    private:

        void enforceName();
        boost::thread bThread;
        bool hasname;
        std::string name;


	};

};

#endif // SYSTEMTHREAD_HPP
